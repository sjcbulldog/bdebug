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
            GDBServerMCU(GDBServerBackend *parent, const std::string &tag);
            virtual ~GDBServerMCU();

            bool connectSocket(const char *addr, uint16_t port);
            bool queryDevice();
            bool initialize();

            std::string cpuTypeName() const;

            bool readMemory(uint32_t addr, uint32_t& value);
            bool readMemory(uint32_t addr, uint32_t length, std::vector<uint32_t>& data);
            bool readRegisters();
            
            bool readVectorTable() override ;
            bool run(bool wait = false) override;
            bool restart() override;
            bool setBreakpoint(BreakpointType type, uint32_t addr, uint32_t size) override;

        private:
            std::string sendPacketGetResponse(const std::string& pkt);
            bool sendPacket(const std::string& packet);
            bool validPacket(const std::string& str, std::string& payload);

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

            static std::list<std::string> register_names_;

        };
    }
}


