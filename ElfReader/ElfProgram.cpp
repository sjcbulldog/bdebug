#include "ElfProgram.h"

namespace bwg
{
	namespace elf
	{
		ElfProgram::ElfProgram(uint64_t offset, uint64_t vaddr, uint64_t paddr, uint64_t filesz, uint64_t memsz, uint64_t align)
		{
			offset_ = offset;
			vaddr_ = vaddr;
			paddr_ = paddr;
			filesz_ = filesz;
			memsz_ = memsz;
			align_ = align;
		}

		ElfProgram::~ElfProgram()
		{
		}
	}
}
