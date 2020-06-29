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

            bool restart() override;
            bool run(const std::string& tag) override;
            bool connect() override;
            bool stop(const std::string& mcu) override;

            std::list<std::string> mcuTags() override {
                std::list<std::string> ret;

                for (auto pair : descs())
                    ret.push_back(pair.first);

                return ret;
            }

            const MCUDesc& desc(const std::string& tag) const override {
                auto it = descs().find(tag);
                assert(it != descs().end());

                return it->second;
            }

            bool initPhaseTwo() override;

        private:
            bool connectMCUs(const std::filesystem::path& config_file, nlohmann::json& obj);
            bool startProgram(const std::filesystem::path& config_file, nlohmann::json& obj);
            void readOut(const char* data, size_t n);
            void readErr(const char* data, size_t n);

        protected:
            constexpr static const char* ModuleName = "gdbserver-backend";
            constexpr static const char* JsonNameStartupDelay = "delay";
            constexpr static const char* JsonNameMCUs = "mcus";
            constexpr static const char* JsonNameTag = "tag";
            constexpr static const char* JsonNameBackEndPort = "beport";
            constexpr static const char* JsonNameArgs = "args";

        private:
            std::filesystem::path exepath_;
            std::string name_;
            bwg::platform::PlatformProcess* process_;
            std::string out_;
            std::string err_;
        };
    }
}
