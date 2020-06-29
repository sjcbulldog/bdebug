#pragma once

#include <filesystem>
#include <string>

namespace bwg
{
	namespace platform
	{
		class PlatformLocations
		{
		public:
			PlatformLocations() = delete;
			~PlatformLocations() = delete;

			static std::filesystem::path configDirectory();
			static std::filesystem::path exeLocation();
		};
	}
}
