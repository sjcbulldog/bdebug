#pragma once

#include <string>

namespace bwg
{
	namespace backend
	{
		class MCUDesc
		{
		public:
			MCUDesc(const std::string& type) {
				devtype_ = type;
			}

			virtual ~MCUDesc() {
			}

			const std::string& type() const {
				return devtype_;
			}

		private:
			std::string devtype_;
		};
	}
}
