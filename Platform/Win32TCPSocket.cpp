#include "Win32TCPSocket.h"
#include <stdexcept>
#include <WS2tcpip.h>

namespace bwg
{
	namespace platform
	{
		bool Win32TCPSocket::winsock_started_ = false;

		Win32TCPSocket::Win32TCPSocket()
		{
			if (!winsock_started_)
			{
				WSADATA wsaData;

				if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
				{
					std::runtime_error err("cannot start Windows Sockets");
					throw err;
				}
			}

			socket_ = INVALID_SOCKET;
			buffer_.resize(1536);
		}

		Win32TCPSocket::~Win32TCPSocket()
		{
			if (socket_ != INVALID_SOCKET)
				::shutdown(socket_, SD_SEND);

			socket_ = INVALID_SOCKET;
		}

		bool Win32TCPSocket::connect(const std::string& addr, uint16_t port)
		{
			struct addrinfo* result = NULL, * ptr = NULL, hints;

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			std::string portstr = std::to_string(port);
			int retcore = getaddrinfo(addr.c_str(), portstr.c_str(), &hints, &result);
			if (retcore != 0)
			{
				setOsError(static_cast<uint32_t>(retcore));
				return false;
			}

			bool ret = false;
			for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
			{
				socket_ = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
				if (socket_ == INVALID_SOCKET)
					continue;

				if (::connect(socket_, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR)
				{
					closesocket(socket_);
					socket_ = INVALID_SOCKET;
					continue;
				}

				ret = true;
				break;
			}

			freeaddrinfo(result);

			return ret;
		}

		bool Win32TCPSocket::hasData()
		{
			unsigned long value;

			if (ioctlsocket(socket_, FIONREAD, &value) != 0)
				return false;

			return value > 0;
		}

		int Win32TCPSocket::readOne()
		{
			char ch;
			int res;

			res = recv(socket_, &ch, 1, 0);
			if (res < 0)
			{
				setOsError(GetLastError());
				return res;
			}

			return ch;
		}

		int Win32TCPSocket::read(std::vector<uint8_t>& data)
		{
			int res;

			res = recv(socket_, (char *)&buffer_[0], (int)buffer_.size(), 0);
			if (res == SOCKET_ERROR)
			{
				setOsError(GetLastError());
				return -1;
			}

			if (res != 0)
			{
				data.resize(res);
				memcpy(&data[0], &buffer_[0], res);
			}

			return res;
		}

		int Win32TCPSocket::write(const std::vector<uint8_t>& data)
		{
			int res;
			int index = 0;
			int remaining = (int)data.size();

			while (remaining > 0)
			{
				res = ::send(socket_, (const char*)&data[index], remaining, 0);
				if (res == SOCKET_ERROR)
				{
					setOsError(GetLastError());
					return -1;
				}

				remaining -= res;
				index += res;
			}

			return index;
		}

		int Win32TCPSocket::writeOne(char ch)
		{
			int res;

			res = ::send(socket_, &ch, 1, 0);
			if (res < 0)
			{
				setOsError(GetLastError());
				return res;
			}

			return 0;
		}
	}
}