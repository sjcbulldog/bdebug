#pragma once

#include "Destination.h"

namespace bwg
{
	namespace debug
	{
		class UserInputOutputDevice;

		class UIOMessageDestination : public bwg::logfile::Destination
		{
		public:
			UIOMessageDestination(UserInputOutputDevice& iodev);
			virtual ~UIOMessageDestination();

			void send(const bwg::logfile::Message& msg) override;

		private:
			UserInputOutputDevice& iodev_;
		};
	}
}
