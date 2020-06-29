#pragma once

#include "Destination.h"
#include <ostream>

namespace bwg
{
	namespace logger
	{
		class OStreamDestination : public Destination
		{
		public:
			OStreamDestination(std::ostream& strm);
			virtual ~OStreamDestination();

			void send(const Message& msg) override;

		private:
			std::ostream& out_;
		};
	}
}

