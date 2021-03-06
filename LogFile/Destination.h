#pragma once

namespace bwg
{
	namespace logfile
	{
		class Message;

		class Destination
		{
		public:
			Destination();
			virtual ~Destination();

			virtual void send(const Message& msg) = 0 ;
		};
	}
}
