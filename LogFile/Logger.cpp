#include "Logger.h"
#include "Message.h"
#include "Destination.h"

namespace bwg
{
	namespace logger
	{
		Logger::Logger()
		{
			enable(Message::Type::Info);
			enable(Message::Type::Warning);
			enable(Message::Type::Error);
			enable(Message::Type::Fatal);
		}

		Logger::~Logger()
		{
		}

		Logger& Logger::operator<<(const Message& msg)
		{
			bool send = false;

			if (enabled_types_.find(msg.type()) != enabled_types_.end())
				send = true;

			if (enabled_modules_.find(msg.module()) != enabled_modules_.end())
				send = true;

			if (send)
			{
				for (auto dest : destinations_)
					dest->send(msg);
			}

			return *this;
		}

		void Logger::enable(Message::Type type)
		{
			enabled_types_.insert(type);
		}

		void Logger::disable(Message::Type type)
		{
			enabled_types_.erase(type);
		}

		void Logger::enable(const std::string &module)
		{
			enabled_modules_.insert(module);
		}

		void Logger::disable(const std::string& module)
		{
			enabled_modules_.erase(module);
		}
	}
}
