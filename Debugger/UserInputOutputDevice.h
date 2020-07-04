#pragma once

#include <string>

#ifdef _MSC_VER
#include <Windows.h>
#endif

namespace bwg
{
	namespace debug
	{
		class UserInputOutputDevice
		{
		public:
			UserInputOutputDevice();
			virtual ~UserInputOutputDevice();

			void setPrompty(const std::string& prompt) {
				prompt_ = prompt;
			}

			bool getLine(std::string& line);

			bool isInterruptActive() const {
				return interrupt_pressed_;
			}

			void clearInterruptActive() {
				interrupt_pressed_ = false;
			}

			void setInterruptActive() {
				interrupt_pressed_ = true;
			}

			void putString(const std::string& str);

		private:
			static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

		private:
			std::string prompt_;
			bool interrupt_pressed_;

			static UserInputOutputDevice* theOne;
		};
	}
}
