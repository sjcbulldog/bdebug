#pragma once

#include "BackendMCU.h"
#include "NetworkTCPSocket.h"
#include "cortexreg.h"
#include <string>
#include <list>
#include <map>
#include <thread>

namespace bwg
{
    namespace backend
    {
        class GDBServerBackend;

        class GDBServerMCU : public BackendMCU
        {
        public:
            GDBServerMCU(GDBServerBackend* parent, const std::string& mcutag, bool master, const std::string& hostaddr, uint16_t port);
            virtual ~GDBServerMCU();

            bool connectSocket(const char *addr, uint16_t port);
            bool queryDevice();
            bool initialize();

            std::string cpuTypeName() const;

            bool readMemory(uint32_t addr, uint32_t& value);
            bool readMemory(uint32_t addr, uint32_t length, std::vector<uint32_t>& data);
            bool readRegisters();
            
        private:
            void mcuThread(const std::string &hostaddr, uint16_t port);
            bool validPacket(const std::string& str, std::string& payload);
            bool sendPacketGetResponse(const std::string& pkt, std::string& resp);
            bool sendPacket(const std::string& packet);
            bool receivePacket(std::string& packet);
            bool sendRemoteCommand(const std::string& command, std::string& resp);

            std::string encodeHex(const std::string& text);
            std::string decodeHex(const std::string& hex);

            bool setBreakpoint(BreakpointType type, uint64_t addr, uint64_t size);
            bool removeBreakpoint(BreakpointType type, uint64_t addr, uint64_t size);
            bool provideSymbols();
            bool reset();
            bool stop();
            bool run();

            void doRequest(std::shared_ptr<DebuggerRequest> req);

        private:
            std::thread* thread_;
            bwg::platform::NetworkTCPSocket* socket_;
            uint16_t port_;
            bool ack_mode_;
            std::string supported_;

            uint16_t cputype_;

            std::map<std::string, uint32_t> registers_;

            uint32_t reset_;
            uint32_t main_;
            std::string vector_table_;

            std::map<std::string, std::list<std::string>> cmds_;

            static std::list<std::string> register_names_;
        };
    }
}


