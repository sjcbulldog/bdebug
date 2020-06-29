#include "GDBServerMCU.h"
#include "GDBServerBackend.h"
#include "Message.h"
#include "MiscUtilsHex.h"
#include "platcompiler.h"
#include <thread>

using namespace bwg::platform;
using namespace bwg::logger;
using namespace bwg::misc;

namespace bwg
{
	namespace backend
	{
        //
        // The data back from the 'g' packet for the registers
        //
        std::list<std::string> GDBServerMCU::register_names_ = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc", "xPSR" };

        GDBServerMCU::GDBServerMCU(GDBServerBackend *parent, const std::string &tag) : BackendMCU(tag)
        {
            parent_ = parent;
            socket_ = nullptr;
            state_ = 0;
            port_ = 0xFFFF;
            cputype_ = 0xFFFF;
            ack_mode_ = true;
            vector_table_ = "__Vectors";
            reset_ = 0;
            main_ = 0;
        }

        GDBServerMCU::~GDBServerMCU()
        {
            if (socket_ != nullptr)
                delete socket_;
        }

        bool GDBServerMCU::validPacket(const std::string& packet, std::string& payload)
        {
            int index = 0;
            payload.clear();

            while (packet[index] == '+')
                index++;

            if (packet[index++] != '$')
                return false;

            uint8_t sum = 0;
            while (index < packet.length() && packet[index] != '#')
            {
                payload += packet[index];
                sum += static_cast<uint8_t>(packet[index]);
                index++;
            }

            if (index == packet.length())
                return false;

            index++;

            if (index > packet.length() - 2)
                return false;

            std::string hex = MiscUtilsHex::n2hexstr<uint8_t>(sum);
            if (hex[0] != packet[index] || hex[1] != packet[index + 1])
                return false;

            return true;
        }

        bool GDBServerMCU::sendPacket(const std::string& pkt)
        {
            uint8_t sum = 0;
            std::vector<uint8_t> data(pkt.length() + 4);


            data[0] = '$';
            memcpy(&data[1], &pkt[0], pkt.length());
            data[pkt.length() + 1] = '#';

            for (char ch : pkt)
                sum += static_cast<uint8_t>(ch);

            std::string hex = MiscUtilsHex::n2hexstr<uint8_t>(sum);
            data[pkt.length() + 2] = hex[0];
            data[pkt.length() + 3] = hex[1];

            if (socket_->write(data) == -1)
                return false;

            return true;
        }

        std::string GDBServerMCU::sendPacketGetResponse(const std::string& pkt)
        {
            std::string ret;
            std::vector<uint8_t> response;

            if (!sendPacket(pkt))
                return ret;

            std::string answer, payload;
            while (!validPacket(answer, payload))
            {
                if (socket_->read(response) == -1)
                    return ret;

                for (int i = 0; i < response.size(); i++)
                    answer += (char)response[i];
            }

            return payload;
        }

        bool GDBServerMCU::sendRemoteCommand(const std::string& command)
        {
            return true;
        }

        std::string GDBServerMCU::cpuTypeName() const
        {
            std::string ret = "Unknown";

            switch (cputype_)
            {
            case ARM_CPUID_PARTNO_CM0:
                ret = "Cortex-M0";
                break;
            case ARM_CPUID_PARTNO_CM0P:
                ret = "Cortex-M0P";
                break;
            case ARM_CPUID_PARTNO_CM3:
                ret = "Cortex-M3";
                break;
            case ARM_CPUID_PARTNO_CM4:
                ret = "Cortex-M4";
                break;
            case ARM_CPUID_PARTNO_CM23:
                ret = "Cortex-M23";
                break;
            case ARM_CPUID_PARTNO_CM33:
                ret = "Cortex-M33";
                break;
            case ARM_CPUID_PARTNO_CM7:
                ret = "Cortex-M7";
                break;
            }

            return ret;
        }

        bool GDBServerMCU::connectSocket(const char* addr, uint16_t port)
        {
            bool connected = false;

            socket_ = NetworkTCPSocket::createSocket();

            for (int i = 0; i < 10; i++)
            {
                if (socket_->connect(addr, static_cast<uint16_t>(port)))
                {
                    std::string resp = sendPacketGetResponse("!");
                    if (resp == "OK")
                    {
                        connected = true;
                        port_ = port;
                    }
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
            return connected;
        }

        bool GDBServerMCU::readMemory(uint32_t addr, uint32_t& value)
        {
            std::string pkt, resp;

            pkt = "m " + MiscUtilsHex::n2hexstr<uint32_t>(addr) + ",4";
            resp = sendPacketGetResponse(pkt);

            if (resp.length() == 0)
                return false;

            value = PLATFORM_SWAPU32(static_cast<uint32_t>(MiscUtilsHex::hexToInt(resp.c_str(), (int)resp.length())));
            return true;
        }

        bool GDBServerMCU::readMemory(uint32_t addr, uint32_t length, std::vector<uint32_t>& data)
        {
            std::string pkt, resp;
            assert((length % 4) == 0);

            pkt = "m " + MiscUtilsHex::n2hexstr<uint32_t>(addr) + "," + MiscUtilsHex::n2hexstr<uint32_t>(length);
            resp = sendPacketGetResponse(pkt);

            if (resp.length() == 0)
                return false;

            data.resize(length);



            return true;
        }

        bool GDBServerMCU::initialize()
        {
            std::string pkt, resp;

            pkt = "qSupported:multiprocess+;swbreak+;hwbreak+;qRelocInsn+;fork-events+;vfork-events+;exec-events+;vContSupported+;QThreadEvents+;no-resumed+";
            supported_ = sendPacketGetResponse(pkt);

            if (supported_.length() == 0)
                return false;

            if (supported_.find("QStartNoAckMode+") != std::string::npos)
            {
                pkt = "QStartNoAckMode";
                resp = sendPacketGetResponse(pkt);
                if (resp.length() == 0)
                    return false;

                if (resp == "OK")
                    ack_mode_ = false;
            }

            if (!readRegisters())
                return false;

            return true;
        }

        bool GDBServerMCU::readRegisters()
        {
            std::string resp;

            resp = sendPacketGetResponse("g");
            if (resp.length() == 0)
                return false;

            int index = 0;
            for (const std::string& reg : register_names_)
            {
                uint32_t v = PLATFORM_SWAPU32(static_cast<uint32_t>(MiscUtilsHex::hexToInt(&resp[index], 8)));
                registers_.insert_or_assign(reg, v);
                index += 8;
            }

            return true;
        }

        bool GDBServerMCU::readVectorTable()
        {
            uint32_t sp, reset;

            auto vtable = parent_->findSymbol(tag(), vector_table_);
            if (vtable == nullptr)
            {
                Message msg(Message::Type::Warning, GDBServerBackend::ModuleName);
                msg << "symbol '" << vector_table_ << "' not found in loaded ELF file, cannot find vector table";
                parent_->logger() << msg;
                return false;
            }

            uint32_t addr = static_cast<uint32_t>(vtable->value());
            if (!readMemory(addr, sp))
            {
                Message msg(Message::Type::Warning, GDBServerBackend::ModuleName);
                msg << "cannot read vector table at address " << bwg::misc::MiscUtilsHex::n2hexstr<uint32_t>(addr);
                parent_->logger() << msg;
                return false;
            }

            addr += 4;
            if (!readMemory(addr, reset))
            {
                Message msg(Message::Type::Warning, GDBServerBackend::ModuleName);
                msg << "cannot read vector table at address " << bwg::misc::MiscUtilsHex::n2hexstr<uint32_t>(addr);
                parent_->logger() << msg;
                return false;
            }

            reset_ = reset;

            return true;
        }

        bool GDBServerMCU::setBreakpoint(BreakpointType type, uint32_t addr, uint32_t size)
        {
            std::string packet, resp;

            switch (type)
            {
            case BreakpointType::Software:
                packet = "z0," + MiscUtilsHex::n2hexstr<uint32_t>(addr) + "," + MiscUtilsHex::n2hexstr<uint32_t>(size);
                break;
            case BreakpointType::Hardware:
                packet = "z1," + MiscUtilsHex::n2hexstr<uint32_t>(addr) + "," + MiscUtilsHex::n2hexstr<uint32_t>(size);
                break;
            case BreakpointType::Write:
                packet = "z2," + MiscUtilsHex::n2hexstr<uint32_t>(addr) + "," + MiscUtilsHex::n2hexstr<uint32_t>(size);
                break;
            case BreakpointType::Read:
                packet = "z3," + MiscUtilsHex::n2hexstr<uint32_t>(addr) + "," + MiscUtilsHex::n2hexstr<uint32_t>(size);
                break;
            case BreakpointType::Access:
                packet = "z4," + MiscUtilsHex::n2hexstr<uint32_t>(addr) + "," + MiscUtilsHex::n2hexstr<uint32_t>(size);
                break;
            }

            resp = sendPacketGetResponse(packet);
            if (resp.length() == 0)
                return false;

            return true;
        }

        bool GDBServerMCU::restart()
        {
            std::string resp;

            readRegisters();
            resp = sendPacket("vCont;c");
            resp = sendPacketGetResponse("vCtrlC");
            resp = sendPacketGetResponse("?");
            readRegisters();

            return true;
        }

        bool GDBServerMCU::run(bool wait)
        {
            if (reset_ == 0)
            {
                if (!readVectorTable())
                    return false;
            }

            if (main_ == 0)
            {
                auto main = parent_->findSymbol(tag(), "main");
                if (main != nullptr)
                    main_ = static_cast<uint32_t>(main->value());
            }

            if (stopAtMain())
                setBreakpoint(BreakpointType::Hardware, main_, 2);

            if (stopAtReset())
                setBreakpoint(BreakpointType::Hardware, reset_, 2);

            if (wait)
            {
                std::string resp = sendPacketGetResponse("c");
                if (resp.length() == 0)
                    return false;
            }
            else
            {
                if (!sendPacket("c"))
                    return false;
            }

            return true;
        }

        bool GDBServerMCU::queryDevice()
        {
            uint32_t value;

            if (!readMemory(ARM_SCB_BASE, value))
                return false;

            cputype_ = (value >> 4) & 0xFFF;

            Message msg(Message::Type::Debug, GDBServerBackend::ModuleName);
            msg << "connected to CPU '" << cpuTypeName() << "' with tag '" << tag() << "' on port '" << port_;
            parent_->logger() << msg;

            return true;
        }

	}
}