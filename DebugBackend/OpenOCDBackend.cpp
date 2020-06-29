#include "OpenOCDBackend.h"
#include "Message.h"
#include "Logger.h"
#include "PlatformProcess.h"

using namespace bwg::logger;
using namespace bwg::platform;

namespace bwg
{
    namespace backend
    {
        OpenOCDBackend::OpenOCDBackend(bwg::logger::Logger& logger, const std::string& name) : GDBServerBackend(logger, name)
        {
            process_ = nullptr;
        }

        OpenOCDBackend::~OpenOCDBackend()
        {
            if (process_ != nullptr)
                delete process_;
        }

        bool OpenOCDBackend::startProgram(const std::filesystem::path& config_file, nlohmann::json& obj)
        {

            if (!obj.contains(JsonNameStartupDelay))
            {
                Message msg(Message::Type::Error, ModuleName);
                msg << "config file '" << config_file << "' is missing the '" << JsonNameStartupDelay << "' field";
                logger() << msg;
                return false;
            }

            if (!obj[JsonNameStartupDelay].is_number_integer())
            {
                Message msg(Message::Type::Error, ModuleName);
                msg << "config file '" << config_file << "' has the '" << JsonNameStartupDelay << "' field, but it is not an integer";
                logger() << msg;
                return false;
            }

            if (!obj.contains(JsonNameScriptDirs))
            {
                Message msg(Message::Type::Error, ModuleName);
                msg << "config file '" << config_file << "' is missing the '" << JsonNameScriptDirs << "' field";
                logger() << msg;
                return false;
            }

            if (!obj[JsonNameScriptDirs].is_array())
            {
                Message msg(Message::Type::Error, ModuleName);
                msg << "config file '" << config_file << "' has the '" << JsonNameScriptDirs << "' field, but it is not a string";
                logger() << msg;
                return false;
            }

            if (!obj.contains(JsonNameCommands))
            {
                Message msg(Message::Type::Error, ModuleName);
                msg << "config file '" << config_file << "' is missing the '" << JsonNameCommands << "' field";
                logger() << msg;
                return false;
            }

            if (!obj[JsonNameCommands].is_array())
            {
                Message msg(Message::Type::Error, ModuleName);
                msg << "config file '" << config_file << "' has the '" << JsonNameCommands << "' field, but it is not a string";
                logger() << msg;
                return false;
            }

            std::vector<std::string> args;
            args.push_back(exePath().generic_string());

            auto array = obj[JsonNameScriptDirs];
            for (auto it = array.begin(); it != array.end(); it++)
            {
                auto strobj = *it;
                if (strobj.is_string())
                {
                    args.push_back("-s");
                    args.push_back("\"" + strobj.get<std::string>() + "\"");
                }
            }

            array = obj[JsonNameCommands];
            for (auto it = array.begin(); it != array.end(); it++)
            {
                nlohmann::json strobj = *it;
                if (strobj.is_string())
                {
                    args.push_back("-c");
                    args.push_back("\"" + strobj.get<std::string>() + "\"");
                }
            }

            //
            // TODO - this assumes something about OpenOCD and is not GDBServer specific
            //
            std::string path = exePath().parent_path().parent_path().generic_string();
            for (size_t i = 0; i < args.size(); i++)
            {
                auto index = args[i].find("$$PATH$$");
                if (index != std::string::npos)
                {
                    std::string newstr = args[i].substr(0, index);
                    newstr += path;
                    newstr += args[i].substr(index + 8);
                    args[i] = newstr;
                }
            }

            auto outcb = std::bind(&OpenOCDBackend::readOut, this, std::placeholders::_1, std::placeholders::_2);
            auto outerr = std::bind(&OpenOCDBackend::readErr, this, std::placeholders::_1, std::placeholders::_2);
            process_ = new PlatformProcess(args, outcb, outerr, true);

            if (!process_->didStart())
            {
                delete process_;
                process_ = nullptr;
                return false;
            }

            return true;
        }

        void OpenOCDBackend::readOut(const char* str, size_t n)
        {
            for (int i = 0; i < n; i++)
            {
                if (str[i] == '\n')
                {

                    Message msg(Message::Type::Debug, "openocd");
                    msg << out_;
                    logger() << msg;
                    out_.clear();
                }
                else
                {
                    out_ += str[i];
                }
            }
        }

        void OpenOCDBackend::readErr(const char* str, size_t n)
        {
            for (int i = 0; i < n; i++)
            {
                if (str[i] == '\n')
                {

                    Message msg(Message::Type::Debug, "openocd");
                    msg << out_;
                    logger() << msg;
                    err_.clear();
                }
                else
                {
                    err_ += str[i];
                }
            }
        }
    }
}
