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
			virtual bool hasData() = 0;
			virtual int read(std::vector<uint8_t>& data) = 0 ;
			virtual int readOne() = 0 ;

			virtual int write(const std::vector<uint8_t>& data) = 0;
			virtual int writeOne(char ch) = 0 ;

			uint32_t osError() const {
				return os_error_;
			}

			static NetworkTCPSocket* createSocket();

		protected:
			void setOsError(uint32_t e) {
				os_error_ = e;
			}


		private:
			uint32_t os_error_;
		};
	}
}

