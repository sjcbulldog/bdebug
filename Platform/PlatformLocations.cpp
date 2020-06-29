#include "PlatformLocations.h"

#include <vector>

#ifdef _MSC_VER
#include <Windows.h>
#endif

namespace bwg
{
	namespace platform
	{

#ifdef _MSC_VER
		std::filesystem::path PlatformLocations::configDirectory()
		{
			std::filesystem::path p = exeLocation();
#ifdef _DEBUG
			p = p.parent_path().parent_path().parent_path() / "config";
#else
			p = p.parent_path() / "config";
#endif
			return p;
		}

		std::filesystem::path PlatformLocations::exeLocation()
		{
			std::filesystem::path ret;
			std::vector<char> filename(MAX_PATH);

			DWORD len = GetModuleFileNameA(NULL, static_cast<LPSTR>(&filename[0]), static_cast<DWORD>(filename.size()));
			if (len < filename.size() || GetLastError() != ERROR_INSUFFICIENT_BUFFER)
				ret = &filename[0];

			return ret;
		}
#endif
	}
}