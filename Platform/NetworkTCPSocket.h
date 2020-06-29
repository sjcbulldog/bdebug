#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace bwg
{
	namespace platform
	{
		class NetworkTCPSocket
		{
		public:
			NetworkTCPSocket();
			virtual ~NetworkTCPSocket();

			virtual bool connect(const std::string& addr, uint16_t port) = 0;
			virtual int read(std::vector<uint8_t>& data) = 0 ;
			virtual int write(const std::vector<uint8_t>& data) = 0;

			static NetworkTCPSocket* createSocket();

		private:
		};
	}
}

