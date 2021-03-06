#pragma once
#include "json.hpp"
#include "MCUDesc.h"
#include "DebugBackend.h"
#include "PlatformProcess.h"
#include "NetworkTCPSocket.h"
#include <string>
#include <map>
#include <filesystem>
#include <cassert>
#include <thread>

namespace bwg
{
    namespace backend
    {
        class GDBServerMCU;

        class GDBServerBackend : public DebugBackend
        {
            friend class GDBServerMCU;

        public:
            GDBServerBackend(bwg::logfile::Logger& logger, const std::string& name);
            virtual ~GDBServerBackend();

            void setExePath(const std::filesystem::path& path) {
                exepath_ = path;
            }

            //
            // Connects to the backend and determines the types of MCUs attached
            //
            bool connect() override;

            //
            // Returns true when all of the MCUs are ready
            //
            bool ready() override;

            //
            // Reset the entire device, including all of the MCUs
            //
            bool reset() override;

            //
            // Reset a single device, it may work differently depending on
            // the type of device.  If a device is reset, all of its dependent
            // devices are reset as well.
            //
            bool run(const std::string& mcutag) override;
            bool run();
            bool stop(const std::string& mcu) override;
            bool stop();
            bool setBreakpoint(const std::string& mcutag, BreakpointType type, uint64_t addr, uint64_t size) override ;
            bool removeBreakpoint(const std::string& mcutag, BreakpointType type, uint64_t addr, uint64_t size) override;


        private:
            bool connectMCUs(const std::filesystem::path& config_file, nlohmann::json& obj);
            bool startProgram(const std::filesystem::path& config_file, nlohmann::json& obj);
            void readOut(const char* data, size_t n);
            void readErr(const char* data, size_t n);
            void startMCUThread(const std::string& mcutag, bool master, const char* addr, uint16_t port);

        protected:
            constexpr static const char* ModuleName = "gdbserver-backend";
            constexpr static const char* JsonNameStartupDelay = "delay";
            constexpr static const char* JsonNameMCUs = "mcus";
            constexpr static const char* JsonNameTag = "mcutag";
            constexpr static const char* JsonNameMaster = "master";
            constexpr static const char* JsonNameBackEndPort = "beport";
            constexpr static const char* JsonNameArgs = "args";

        private:
            std::filesystem::path exepath_;
            std::string name_;
            bwg::platform::PlatformProcess* process_;
            std::string out_;
            std::string err_;
            std::map<std::string, std::thread*> threads_;
        };
    }
}
