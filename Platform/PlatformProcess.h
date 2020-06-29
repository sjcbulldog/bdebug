#pragma once

#include <string>
#include <vector>
#include <functional>

namespace bwg
{
	namespace platform
	{
		class PlatformProcess
		{
		public:
			PlatformProcess(std::vector<std::string>& args,
				std::function<void(const char* bytes, size_t n)> read_stdout = nullptr,
				std::function<void(const char* bytes, size_t n)> read_stderr = nullptr,
				bool open_stdin = false);
			virtual ~PlatformProcess();

			bool didStart() {
				return os_process_obj_ != nullptr;
			}

#ifdef _MSC_VER
			static std::wstring convert(const std::string& str);
#endif

		private:
			void* os_process_obj_;
		};
	}
}


