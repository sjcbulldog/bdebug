#pragma once

#include "GDBServerBackend.h"

namespace bwg
{
    namespace backend
    {
        class OpenOCDBackend : public GDBServerBackend
        {
        public:
            OpenOCDBackend(bwg::logger::Logger& logger, const std::string& name);
            virtual ~OpenOCDBackend();

        protected:
            bool startProgram(const std::filesystem::path& config_file, nlohmann::json& obj) override ;

        private:
            constexpr static const char* JsonNameArgsDirs = "scriptdirs";
            constexpr static const char* JsonNameCommands = "commands";

        private:
            void readOut(const char* data, size_t n);
            void readErr(const char* data, size_t n);

        private:
            bwg::platform::PlatformProcess* process_;
            std::string out_;
            std::string err_;
        };
    }
}

