#include "ElfSection.h"

namespace bwg
{
	namespace elf
	{
		ElfSection::ElfSection(const std::string &name)
		{
			name_ = name;
		}

		ElfSection::~ElfSection()
		{
		}
	}
}
