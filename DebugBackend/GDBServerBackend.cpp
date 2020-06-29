#include "GDBServerBackend.h"
#include "GDBServerMCU.h"
#include "PlatformLocations.h"
#include "json.hpp"
#include <thread>
#include <fstream>

using namespace bwg::logger;
using namespace bwg::platform;

namespace bwg
{
    namespace backend
    {
        GDBServerBackend::GDBServerBackend(bwg::logger::Logger& logger, const std::string& name) : DebugBackend(logger)
        {
            name_ = name;
            exepath_ = "openocd";
        }

        GDBServerBackend::~GDBServerBackend()
        {
        }

        bool GDBServerBackend::restart()
        {
            auto it = mcus().begin();
            if (it == mcus().end())
                return false;

            return it->second->restart();
        }

        bool GDBServerBackend::run(const std::string& tag, bool wait)
        {
            auto it = mcus().find(tag);
            if (it == mcus().end())
                return false;

            return it->second->run(wait);
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

            nlohmann::json obj = nlohmann::json::parse(stream);

            if (!startProgram(config_file, obj))
                return false;

            if (!connectMCUs(config_file, obj))
                return false;

            return true;
        }

        bool GDBServerBackend::initPhaseTwo()
        {
            for (auto pair : mcus())
            {
                if (!pair.second->readVectorTable())
                    return false;
            }

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

                    std::string tag = mcu[JsonNameTag].get<std::string>();
                    auto mcuobj = std::make_shared<GDBServerMCU>(this, tag);
                    int port = mcu[JsonNameBackEndPort].get<int>();
                    if (mcuobj->connectSocket("127.0.0.1", static_cast<uint16_t>(port)))
                    {
                        if (mcuobj->initialize())
                        {
                            if (mcuobj->queryDevice())
                            {
                                setMCU(tag, mcuobj);
                                setMCUDesc(tag, MCUDesc(mcuobj->cpuTypeName()));
                            }
                        }
                    }
                }
            }

            return true;
        }

    }
}
