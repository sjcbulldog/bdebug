#pragma once

#include "Destination.h"
#include <ostream>

namespace bwg
{
	namespace logfile
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

