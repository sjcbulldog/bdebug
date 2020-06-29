#include "OStreamDestination.h"
#include "Message.h"

namespace bwg
{
	namespace logfile
	{
		OStreamDestination::OStreamDestination(std::ostream& strm) : out_(strm)
		{
		}

		OStreamDestination::~OStreamDestination()
		{
		}

		void OStreamDestination::send(const Message& msg)
		{
			out_ << msg.messageString() << std::endl;
		}
	}
}
