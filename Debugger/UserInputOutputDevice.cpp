#include "UserInputOutputDevice.h"
#include <iostream>
#include <cassert>


namespace bwg
{
	namespace debug
	{
		UserInputOutputDevice* UserInputOutputDevice::theOne = nullptr;

		UserInputOutputDevice::UserInputOutputDevice()
		{
			assert(theOne == nullptr);
			SetConsoleCtrlHandler(UserInputOutputDevice::CtrlHandler);
		}

		UserInputOutputDevice::~UserInputOutputDevice()
		{
			assert(theOne == this);
			theOne = nullptr;
		}

		BOOL WINAPI UserInputOutputDevice::CtrlHandler(DWORD fdwCtrlType)
		{
			BOOL ret = FALSE;

			switch (fdwCtrlType)
			{
			case CTRL_C_EVENT:
				if (theOne != nullptr)
					theOne->setInterruptActive();

				ret = true;
				break;
			}

			return ret;
		}

		bool UserInputOutputDevice::getLine(std::string& line)
		{
			if (std::cin.eof() || std::cin.bad() || std::cin.fail())
				return false;

			putString(prompt_);
			std::getline(std::cin, line);
			
			return true;
		}

		void UserInputOutputDevice::putString(const std::string& text)
		{
			std::cout << text;
			std::cout.flush();
		}
	}
}
