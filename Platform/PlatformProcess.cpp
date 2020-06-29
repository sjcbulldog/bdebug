#include "PlatformProcess.h"
#include "process.hpp"

#ifdef _MSC_VER
#include <Windows.h>
#endif

namespace bwg
{
	namespace platform
	{
		PlatformProcess::PlatformProcess(std::vector<std::string>& args,
			std::function<void(const char* bytes, size_t n)> read_stdout,
			std::function<void(const char* bytes, size_t n)> read_stderr,
			bool open_stdin)
		{
			std::vector<TinyProcessLib::Process::string_type> wargs;
			TinyProcessLib::Process::string_type path;

#ifdef _MSC_VER
			//
			// On Windows, the OS wants UTF-16, not UTF-8 encoded strings, so we convert the
			// UTF-8 encoded strings to the string windows wants.
			//
			for (const std::string& arg : args)
				wargs.push_back(convert(arg));

#else
			//
			// On linux, mac os ,etc. the std::string object is assumed to contain UTF-8 encoded
			// string which are accepted just find by the OS
			//
			wargs = args;
			path = program;
#endif

			TinyProcessLib::Process* proc = new TinyProcessLib::Process(wargs, path, read_stdout, read_stderr, open_stdin);
			if (proc->get_id() == 0)
			{
				delete proc;
				os_process_obj_ = nullptr;
			}
			else
			{
				os_process_obj_ = proc;
			}
		}

		PlatformProcess::~PlatformProcess()
		{
			if (os_process_obj_ != nullptr)
			{
				TinyProcessLib::Process* proc = static_cast<TinyProcessLib::Process*>(os_process_obj_);
				proc->kill(true);
				delete proc;
				os_process_obj_ = nullptr;
			}
		}

#ifdef _MSC_VER
		std::wstring PlatformProcess::convert(const std::string& str)
		{
			std::vector<wchar_t> wide;

			int len = MultiByteToWideChar(CP_UTF8, MB_COMPOSITE, str.c_str(), -1, nullptr, 0);
			wide.resize(len);
			len = MultiByteToWideChar(CP_UTF8, MB_COMPOSITE, str.c_str(), -1, &wide[0], static_cast<int>(wide.size()));

			return std::wstring(&wide[0]);
		}
#endif
	}
}
