#include "NetworkTCPSocket.h"

#ifdef _MSC_VER
#include "Win32TCPSocket.h"
#endif

namespace bwg
{
	namespace platform
	{
		NetworkTCPSocket::NetworkTCPSocket()
		{
		}

		NetworkTCPSocket::~NetworkTCPSocket()
		{
		}

		NetworkTCPSocket* NetworkTCPSocket::createSocket()
		{
			NetworkTCPSocket* sock = nullptr;

#ifdef _MSC_VER
			sock = new Win32TCPSocket();
#endif

			return sock;
		}
	}
}