#pragma once

#include <string>

namespace bwg
{
	namespace elf
	{
		class ElfSection
		{
		public:
			ElfSection(const std::string &name);
			virtual ~ElfSection();

			const std::string& name() const {
				return name_;
			}

		private:
			std::string name_;
		};
	}
}


