#include "GDBServerMCU.h"
#include "GDBServerBackend.h"
#include "BreakpointRequest.h"
#include "Message.h"
#include "MiscUtilsHex.h"
#include "platcompiler.h"
#include "DebugBackend.h"
#include <thread>

using namespace bwg::platform;
using namespace bwg::logfile;
using namespace bwg::misc;

namespace bwg
{
	namespace backend
	{
        //
        // The data back from the 'g' packet for the registers
        //
        std::list<std::string> GDBServerMCU::register_names_ = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc", "xPSR" };

        GDBServerMCU::GDBServerMCU(GDBServerBackend *parent, const std::string & mcutag, bool master, const std::string &hostaddr, uint16_t port) : BackendMCU(*parent, mcutag, master)
        {
            socket_ = nullptr;
            port_ = 0xFFFF;
            cputype_ = 0xFFFF;
            ack_mode_ = true;
            vector_table_ = "__Vectors";
            reset_ = 0;
            main_ = 0;

            thread_ = new std::thread(&GDBServerMCU::mcuThread, this, hostaddr, port);
        }

        GDBServerMCU::~GDBServerMCU()
        {
            if (socket_ != nullptr)
                delete socket_;
        }

        void GDBServerMCU::doRequest(std::shared_ptr<DebuggerRequest> req)
        {
            bool st = false;

            Message msg(Message::Type::Debug, "backend");
            msg << "GDBServerMCU dequed request " << req->toString();
            backend().logger() << msg;
            req->setStatus(DebuggerRequest::RequestCompletionStatus::Running);

            switch (req->type())
            {
            case DebuggerRequest::BackendRequests::Breakpoint:
            {
                auto bk = std::dynamic_pointer_cast<BreakpointRequest>(req);
                assert(bk != nullptr);
                if (bk->set())
                    setBreakpoint(bk->type(), bk->addr(), bk->size());
                else
                    removeBreakpoint(bk->type(), bk->addr(), bk->size());
            }
            break;
            case DebuggerRequest::BackendRequests::Run:
                st = run();
                break;
            case DebuggerRequest::BackendRequests::Stop:
                st = stop();
                break;
            case DebuggerRequest::BackendRequests::Reset:
                st = reset();
                break;
            default:
                assert(false);
                break;
            }

            if (st)
                req->setStatus(DebuggerRequest::RequestCompletionStatus::Success);
            else
                req->setStatus(DebuggerRequest::RequestCompletionStatus::Error);
        }

        void GDBServerMCU::mcuThread(const std::string& hostaddr, uint16_t port)
        {
            if (!connectSocket(hostaddr.c_str(), port))
            {
                setState(MCUState::FailedConnect);
                return;
            }

            if (!initialize())
            {
                setState(MCUState::FailedInitialization);
                return;
            }

            if (!queryDevice())
            {
                setState(MCUState::FailedInitialization);
                return;
            }

            setState(MCUState::WaitingForReset);

            while (true)
            {
                std::shared_ptr<DebuggerRequest> req;
                bool workdone = false;

                req = next();
                if (req != nullptr)
                {
                    doRequest(req);
                    workdone = true;
                }

                if (socket_->hasData())
                {
                    std::string packet;
                    receivePacket(packet);
                    if (packet[0] == 'T')
                    {
                        setState(MCUState::Stopped);
                    }
                    workdone = true;
                }

                if (!workdone)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }

        std::string GDBServerMCU::encodeHex(const std::string& text)
        {
            std::string result;

            for (char ch : text)
                result += MiscUtilsHex::n2hexstr<uint8_t>(static_cast<uint8_t>(ch));

            return result;
        }

        std::string GDBServerMCU::decodeHex(const std::string& hex)
        {
            std::string result;

            assert((hex.length() % 2) == 0);

            const char* str = hex.c_str();
            for (int i = 0; i < hex.length(); i += 2)
            {
                char ch = static_cast<char>(MiscUtilsHex::hexToInt(str + i, 2));
                result += ch;
            }

            return result;
        }

        bool GDBServerMCU::validPacket(const std::string& packet, std::string& payload)
        {
            int index = 0;
            payload.clear();

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
            int ack;

            Message msg(Message::Type::Debug, "gdbserver");
            msg << mcutag() << ": sending packet '" << pkt << "'";
            backend().logger() << msg;

            data[0] = '$';
            memcpy(&data[1], &pkt[0], pkt.length());
            data[pkt.length() + 1] = '#';

            for (char ch : pkt)
                sum += static_cast<uint8_t>(ch);

            std::string hex = MiscUtilsHex::n2hexstr<uint8_t>(sum);
            data[pkt.length() + 2] = hex[0];
            data[pkt.length() + 3] = hex[1];

            bool ackrecv = !ack_mode_;
            do
            {
                if (socket_->write(data) == -1)
                {
                    msg = Message(Message::Type::Error, "gdbserver");
                    msg << "underlying platform network returned error " << socket_->osError();
                    backend().logger() << msg;
                    return false;
                }

                if (ack_mode_)
                {
                    ack = socket_->readOne();
                    if (ack == -1)
                    {
                        msg = Message(Message::Type::Error, "gdbserver");
                        msg << "underlying platform network returned error " << MiscUtilsHex::n2hexstr<uint32_t>(socket_->osError());
                        backend().logger() << msg;
                        return false;
                    }

                    if (ack == '+')
                        ackrecv = true;
                }

            } while (!ackrecv);

            return true;
        }

        bool GDBServerMCU::receivePacket(std::string& resp)
        {
            std::vector<uint8_t> response;

            std::string answer, payload;
            while (!validPacket(answer, payload))
            {
                if (socket_->read(response) == -1)
                {
                    Message msg(Message::Type::Error, "gdbserver");
                    msg << "underlying platform network returned error " << socket_->osError();
                    backend().logger() << msg;
                    return false;
                }

                for (int i = 0; i < response.size(); i++)
                    answer += (char)response[i];

                while (answer.length() > 0 && answer[0] == '+')
                {
                    //
                    // The GDB Server seems to go ack happy at times, drop any stand alone ack characters.
                    //
                    answer = answer.substr(1);
                }
            }

            if (ack_mode_)
            {
                if (socket_->writeOne('+') == -1)
                {
                    Message msg(Message::Type::Error, "gdbserver");
                    msg << "underlying platform network returned error " << socket_->osError();
                    backend().logger() << msg;
                    return false;
                }
            }

            Message msg(Message::Type::Debug, "gdbserver");
            msg << mcutag() << ": received packet '" << payload << "'";
            backend().logger() << msg;

            if (payload.length() >= 3 && payload.substr(0, 3) == "T05")
            {
                msg = Message(Message::Type::Debug, "gdbserver");
                msg << mcutag() << ": received packet '" << payload << "'";
                backend().logger() << msg;
            }

            resp = payload;
            return true;
        }

        bool GDBServerMCU::sendPacketGetResponse(const std::string& pkt, std::string& resp)
        {
            if (!sendPacket(pkt))
                return false;

            if (!receivePacket(resp))
                return false;

            return true;
        }

        bool GDBServerMCU::sendRemoteCommand(const std::string& command, std::string &resp)
        {
            std::string pktresp;
            std::string tosend;

            resp.clear();

            tosend = "qRcmd," + encodeHex(command);
            if (!sendPacket(tosend))
                return false;

            do {
                if (!receivePacket(pktresp))
                    return false;

                if (pktresp != "OK")
                {
                    if (pktresp.length() > 0 && pktresp[0] == 'O')
                        resp += decodeHex(pktresp.substr(1));
                }

            } while (pktresp != "OK");

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
                    std::string resp;
                    
                    if (!sendPacketGetResponse("!", resp))
                        return false;

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
            if (!sendPacketGetResponse(pkt, resp))
                return false;

            value = PLATFORM_SWAPU32(static_cast<uint32_t>(MiscUtilsHex::hexToInt(resp.c_str(), (int)resp.length())));
            return true;
        }

        bool GDBServerMCU::readMemory(uint32_t addr, uint32_t length, std::vector<uint32_t>& data)
        {
            std::string pkt, resp;
            assert((length % 4) == 0);

            pkt = "m " + MiscUtilsHex::n2hexstr<uint32_t>(addr) + "," + MiscUtilsHex::n2hexstr<uint32_t>(length);
            if (!sendPacketGetResponse(pkt, resp))
                return false;

            data.resize(length);

            // TODO - write the decoed
            assert(false);

            return true;
        }

        bool GDBServerMCU::initialize()
        {
            std::string pkt, resp;

            pkt = "qSupported:multiprocess+;swbreak+;hwbreak+;qRelocInsn+;fork-events+;vfork-events+;exec-events+;vContSupported+;QThreadEvents+;no-resumed+";

            if (!sendPacketGetResponse(pkt, supported_))
                return false;

            if (supported_.length() == 0)
                return false;

            if (supported_.find("QStartNoAckMode+") != std::string::npos)
            {
                pkt = "QStartNoAckMode";
                if (!sendPacketGetResponse(pkt, resp))
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

            if (!sendPacketGetResponse("g", resp))
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

        bool GDBServerMCU::setBreakpoint(BreakpointType type, uint64_t addr, uint64_t size)
        {
            std::string packet, resp;

            switch (type)
            {
            case BreakpointType::Software:
                packet = "Z0," + MiscUtilsHex::n2hexstr<uint64_t>(addr) + "," + std::to_string(size);
                break;
            case BreakpointType::Hardware:
                packet = "Z1," + MiscUtilsHex::n2hexstr<uint64_t>(addr) + "," + std::to_string(size);
                break;
            case BreakpointType::Write:
                packet = "Z2," + MiscUtilsHex::n2hexstr<uint64_t>(addr) + "," + std::to_string(size);
                break;
            case BreakpointType::Read:
                packet = "Z3," + MiscUtilsHex::n2hexstr<uint64_t>(addr) + "," + std::to_string(size);
                break;
            case BreakpointType::Access:
                packet = "Z4," + MiscUtilsHex::n2hexstr<uint64_t>(addr) + "," + std::to_string(size);
                break;
            }

            if (!sendPacketGetResponse(packet, resp))
                return false;

            return true;
        }

        bool GDBServerMCU::removeBreakpoint(BreakpointType type, uint64_t addr, uint64_t size)
        {
            std::string packet, resp;

            switch (type)
            {
            case BreakpointType::Software:
                packet = "z0," + MiscUtilsHex::n2hexstr<uint64_t>(addr) + "," + std::to_string(size);
                break;
            case BreakpointType::Hardware:
                packet = "z1," + MiscUtilsHex::n2hexstr<uint64_t>(addr) + "," + std::to_string(size);
                break;
            case BreakpointType::Write:
                packet = "z2," + MiscUtilsHex::n2hexstr<uint64_t>(addr) + "," + std::to_string(size);
                break;
            case BreakpointType::Read:
                packet = "z3," + MiscUtilsHex::n2hexstr<uint64_t>(addr) + "," + std::to_string(size);
                break;
            case BreakpointType::Access:
                packet = "z4," + MiscUtilsHex::n2hexstr<uint64_t>(addr) + "," + std::to_string(size);
                break;
            }

            if (!sendPacketGetResponse(packet, resp))
                return false;

            return true;
        }

        bool GDBServerMCU::provideSymbols()
        {
            sendPacket("qSymbol::");

            while (true)
            {
                std::string resp;
                if (!receivePacket(resp))
                    return false;

                if (resp == "OK")
                    break;

                auto index = resp.find(':');
                if (index == std::string::npos)
                    return false;

                resp = decodeHex(resp.substr(index + 1));
                auto symbol = backend().findSymbol(mcutag(), resp);
                
                std::string reply = "qSymbol:";
                if (symbol == nullptr)
                {
                    reply += ":";
                }
                else
                {
                    reply += MiscUtilsHex::n2hexstr<uint32_t>(static_cast<uint32_t>(symbol->value())) + ":";
                }
                reply += encodeHex(resp);

                if (!sendPacket(reply))
                    return false;
            }

            return true;
        }

        bool GDBServerMCU::reset()
        {
            std::string resp;
            Message msg(Message::Type::Debug, GDBServerBackend::ModuleName);

            if (mcutag() == "cm0p")
            {
                //
                // TODO - this needs to come from the JSON file
                //
                if (!sendRemoteCommand("psoc6 reset_halt sysresetreq", resp))
                    return false;

                msg.clear();
                msg << resp;
                backend().logger() << msg;


                if (!sendPacketGetResponse("?", resp))
                    return false;

                readRegisters();
            }

            return true;
        }

        bool GDBServerMCU::stop()
        {
            std::string resp;
            std::vector<uint8_t> data;

            data.push_back(0x03);
            socket_->write(data);

            if (!sendPacketGetResponse("?", resp))
                return false;

            setState(MCUState::Stopped);

            readRegisters();

            return true;
        }

        bool GDBServerMCU::run()
        {
            std::string resp;

            if (state() == MCUState::Reset && mcutag() == "cm0p")
            {
                //
                // TODO - this needs to come from the JSON file
                //
                if (!sendRemoteCommand("reset run", resp))
                    return false;
            }
            else
            {
                if (!sendPacket("vCont;c"))
                    return false;
            }

            setState(MCUState::Running);

            return true;
        }

        bool GDBServerMCU::queryDevice()
        {
            uint32_t value;

            if (!readMemory(ARM_SCB_BASE, value))
                return false;

            cputype_ = (value >> 4) & 0xFFF;

            Message msg(Message::Type::Debug, GDBServerBackend::ModuleName);
            msg << "connected to CPU '" << cpuTypeName() << "' with mcutag '" << mcutag() << "' on port '" << port_;
            backend().logger() << msg;

            return true;
        }

	}
}