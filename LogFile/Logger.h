#pragma once

#include "Message.h"
#include <list>
#include <memory>
#include <set>

namespace bwg
{
	namespace logfile
	{
		class Destination;

		class Logger
		{
		public:
			Logger();
			virtual ~Logger();

			Logger& operator<<(const Message& msg);

			void addDestination(std::shared_ptr<Destination> dest) {
				destinations_.push_back(dest);
			}

			void clearDestinations() {
				destinations_.clear();
			}

			void enable(Message::Type type);
			void disable(Message::Type type);

			void enable(const std::string& module);
			void disable(const std::string& module);

		private:
			std::list<std::shared_ptr<Destination>> destinations_;
			std::set<Message::Type> enabled_types_;
			std::set<std::string> enabled_modules_;
		};
	}
}

