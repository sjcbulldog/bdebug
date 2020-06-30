#pragma once

#include "BackendMCU.h"
#include "NetworkTCPSocket.h"
#include "cortexreg.h"
#include <string>
#include <list>
#include <map>

namespace bwg
{
    namespace backend
    {
        class GDBServerBackend;

        class GDBServerMCU : public BackendMCU
        {
        public:
            GDBServerMCU(GDBServerBackend *parent, const std::string & mcutag);
            virtual ~GDBServerMCU();

            bool connectSocket(const char *addr, uint16_t port);
            bool queryDevice();
            bool initialize();

            std::string cpuTypeName() const;

            bool readMemory(uint32_t addr, uint32_t& value);
            bool readMemory(uint32_t addr, uint32_t length, std::vector<uint32_t>& data);
            bool readRegisters();
            
            bool provideSymbols() override;
            bool readVectorTable() override ;
            bool setThreadParams() override;
            bool run() override;
            bool reset() override;
            bool stop() override;
            bool waitForStop() override;
            bool setBreakpoint(BreakpointType type, uint32_t addr, uint32_t size) override;

        private:
            bool validPacket(const std::string& str, std::string& payload);
            bool sendPacketGetResponse(const std::string& pkt, std::string& resp);
            bool sendPacket(const std::string& packet);
            bool receivePacket(std::string& packet);
            bool sendRemoteCommand(const std::string& command, std::string& resp);

            std::string encodeHex(const std::string& text);
            std::string decodeHex(const std::string& hex);

        private:
            GDBServerBackend* parent_;
            bwg::platform::NetworkTCPSocket* socket_;
            uint16_t port_;
            bool ack_mode_;
            std::string supported_;

            uint8_t state_;
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


