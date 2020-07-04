#include "GDBServerBackend.h"
#include "GDBServerMCU.h"
#include "PlatformLocations.h"
#include "DebuggerRequest.h"
#include "json.hpp"
#include <thread>
#include <fstream>

using namespace bwg::logfile;
using namespace bwg::platform;

namespace bwg
{
    namespace backend
    {
        GDBServerBackend::GDBServerBackend(bwg::logfile::Logger& logger, const std::string& name) : DebugBackend(logger)
        {
            name_ = name;
            exepath_ = "openocd";
            process_ = nullptr;
        }

        GDBServerBackend::~GDBServerBackend()
        {
            if (process_ != nullptr)
                delete process_;
        }

        bool GDBServerBackend::ready()
        {
            bool ret = true;

            for (auto pair : mcus())
            {
                if (pair.second->state() != BackendMCU::MCUState::Running)
                    ret = false;
            }

            return ret;
        }

        bool GDBServerBackend::startProgram(const std::filesystem::path& config_file, nlohmann::json& obj)
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

            if (!obj.contains(JsonNameArgs))
            {
                Message msg(Message::Type::Error, ModuleName);
                msg << "config file '" << config_file << "' is missing the '" << JsonNameArgs << "' field";
                logger() << msg;
                return false;
            }

            if (!obj[JsonNameArgs].is_array())
            {
                Message msg(Message::Type::Error, ModuleName);
                msg << "config file '" << config_file << "' has the '" << JsonNameArgs << "' field, but it is not a string";
                logger() << msg;
                return false;
            }

            std::vector<std::string> args;
            args.push_back(exepath_.generic_string());

            //
            // Copy the args from the JSON
            //
            auto array = obj[JsonNameArgs];
            for (auto it = array.begin(); it != array.end(); it++)
            {
                auto strobj = *it;
                if (strobj.is_string())
                    args.push_back(strobj.get<std::string>());
            }

            //
            // Do any path substitution
            //
            std::string token = "$$EXEPATH$$";
            std::string path = exepath_.parent_path().parent_path().generic_string();
            for (size_t i = 0; i < args.size(); i++)
            {
                auto index = args[i].find(token);
                if (index != std::string::npos)
                {
                    std::string newstr = args[i].substr(0, index);
                    newstr += path;
                    newstr += args[i].substr(index + token.length());
                    args[i] = newstr;
                }
            }

            //
            // Quote any args that contain spaces
            //
            for (size_t i = 0; i < args.size(); i++)
            {
                std::string arg = args[i];
                auto pos = arg.find(' ');
                if (pos != std::string::npos)
                {
                    arg = "\"" + arg + "\"";
                    args[i] = arg;
                }
            }

            auto outcb = std::bind(&GDBServerBackend::readOut, this, std::placeholders::_1, std::placeholders::_2);
            auto outerr = std::bind(&GDBServerBackend::readErr, this, std::placeholders::_1, std::placeholders::_2);
            process_ = new PlatformProcess(args, outcb, outerr, true);

            if (!process_->didStart())
            {
                delete process_;
                process_ = nullptr;
                return false;
            }

            return true;
        }

        void GDBServerBackend::readOut(const char* str, size_t n)
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

        void GDBServerBackend::readErr(const char* str, size_t n)
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

        bool GDBServerBackend::reset()
        {
            for (auto pair : mcus())
            {
                if (pair.second->isMaster())
                {
                    auto req = pair.second->request(DebuggerRequest::BackendRequests::Reset);
                    req->waitFor();
                    return true;
                }
            }

            return false;
        }

        bool GDBServerBackend::stop(const std::string& mcutag)
        {
            auto it = mcus().find(mcutag);
            if (it == mcus().end())
                return false;

            auto req = it->second->request(DebuggerRequest::BackendRequests::Stop);
            return req->waitFor();
        }

        bool GDBServerBackend::run(const std::string& mcutag)
        {
            auto it = mcus().find(mcutag);
            if (it == mcus().end())
                return false;

            auto req = it->second->request(DebuggerRequest::BackendRequests::Run);
            return req->waitFor();
        }

        bool GDBServerBackend::setBreakpoint(const std::string &mcutag, BreakpointType type, uint32_t addr, uint32_t size)
        {
            (void)type;
            (void)addr;
            (void)size;

            auto it = mcus().find(mcutag);
            if (it == mcus().end())
                return false;

            // auto req = it->second->request(DebuggerRequest::BackendRequests::Breakpoint, type, addr, size);
            auto req = it->second->request(DebuggerRequest::BackendRequests::Breakpoint);
            return req->waitFor();
        }

        bool GDBServerBackend::connect()
        {
            std::filesystem::path config_file = PlatformLocations::configDirectory() / name_;
            config_file += ".cfg";

            if (!std::filesystem::exists(config_file))
            {
                Message msg(Message::Type::Error, ModuleName);
                msg << "config file '" << config_file << "' does not exists";
                logger() << msg;
                return false;
            }

            if (!std::filesystem::is_regular_file(config_file))
            {
                Message msg(Message::Type::Error, ModuleName);
                msg << "config file '" << config_file << "' exists but is not a file";
                logger() << msg;
                return false;
            }

            std::ifstream stream(config_file);
            if (!stream.is_open())
            {
                Message msg(Message::Type::Error, ModuleName);
                msg << "could not open config file '" << config_file << "' for reading";
                logger() << msg;
                return false;
            }

            nlohmann::json obj;
            
            try {
                obj = nlohmann::json::parse(stream);
            }
            catch (const nlohmann::detail::exception& ex)
            {
                Message msg(Message::Type::Error, ModuleName);
                msg << "error reading JSON config file '" << config_file << "' - " << ex.what();
                logger() << msg;
                return false;
            }

            if (!startProgram(config_file, obj))
                return false;

            if (!connectMCUs(config_file, obj))
                return false;

            return true;
        }

        bool GDBServerBackend::connectMCUs(const std::filesystem::path& config_file, nlohmann::json& obj)
        {
            //
            // Now, delay per the config file for the network port presented by the backend
            // program to be active.
            //
            int delay = obj[JsonNameStartupDelay].get<int>();
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));

            auto array = obj[JsonNameMCUs];
            for (auto it = array.begin(); it != array.end(); it++)
            {
                auto mcu = *it;
                if (mcu.is_object())
                {
                    if (!mcu.contains(JsonNameTag))
                    {
                        Message msg(Message::Type::Error, ModuleName);
                        msg << "config file '" << config_file << "' mcus section, is missing the '" << JsonNameTag << "' field";
                        logger() << msg;
                        return false;
                    }

                    if (!mcu[JsonNameTag].is_string())
                    {
                        Message msg(Message::Type::Error, ModuleName);
                        msg << "config file '" << config_file << "' has the '" << JsonNameTag << "' field, but it is not a string";
                        logger() << msg;
                        return false;
                    }

                    if (!mcu.contains(JsonNameMaster))
                    {
                        Message msg(Message::Type::Error, ModuleName);
                        msg << "config file '" << config_file << "' mcus section, is missing the '" << JsonNameMaster << "' field";
                        logger() << msg;
                        return false;
                    }

                    if (!mcu[JsonNameMaster].is_boolean())
                    {
                        Message msg(Message::Type::Error, ModuleName);
                        msg << "config file '" << config_file << "' has the '" << JsonNameMaster << "' field, but it is not a boolean";
                        logger() << msg;
                        return false;
                    }

                    if (!mcu.contains(JsonNameBackEndPort))
                    {
                        Message msg(Message::Type::Error, ModuleName);
                        msg << "config file '" << config_file << "' mcus section, is missing the '" << JsonNameBackEndPort << "' field";
                        logger() << msg;
                        return false;
                    }

                    if (!mcu[JsonNameBackEndPort].is_number_integer())
                    {
                        Message msg(Message::Type::Error, ModuleName);
                        msg << "config file '" << config_file << "' has the '" << JsonNameBackEndPort << "' field, but it is not a string";
                        logger() << msg;
                        return false;
                    }

                    std::string mcutag = mcu[JsonNameTag].get<std::string>();
                    int port = mcu[JsonNameBackEndPort].get<int>();

                    bool master = mcu[JsonNameMaster].get<bool>();
                    startMCUThread(mcutag, master, "127.0.0.1", static_cast<uint16_t>(port));
                }
            }
            return true;
        }

        void GDBServerBackend::startMCUThread(const std::string &mcutag, bool master, const char* addr, uint16_t port)
        {
            auto mcuobj = std::make_shared<GDBServerMCU>(this, mcutag, master, addr, port);
            setMCU(mcutag, mcuobj);
        }
    }
}
