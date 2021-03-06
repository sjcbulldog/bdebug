#pragma once

#include "NetworkTCPSocket.h"
#include <WinSock2.h>
#include <vector>
#include <cstdint>

namespace bwg
{
	namespace platform
	{
		class Win32TCPSocket : public NetworkTCPSocket
		{
		public:
			Win32TCPSocket();
			virtual ~Win32TCPSocket();

			bool connect(const std::string& addr, uint16_t port) override ;
			virtual bool hasData();
			int read(std::vector<uint8_t>& data) override ;
			int readOne() override;
			int write(const std::vector<uint8_t>& data) override ;
			int writeOne(char ch) override;

		private:
			SOCKET socket_;
			std::vector<uint8_t> buffer_;

			static bool winsock_started_;
		};
	}
}
