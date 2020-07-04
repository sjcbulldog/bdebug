#include "UIOMessageDestination.h"
#include "UserInputOutputDevice.h"
#include "Message.h"

using namespace bwg::logfile;

namespace bwg
{
	namespace debug
	{
		UIOMessageDestination::UIOMessageDestination(UserInputOutputDevice& iodev) : iodev_(iodev)
		{
		}

		UIOMessageDestination::~UIOMessageDestination()
		{
		}

		void UIOMessageDestination::send(const Message& msg)
		{
			iodev_.putString(msg.messageString() + "\n");
		}
	}
}
