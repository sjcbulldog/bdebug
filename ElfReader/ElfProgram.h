#pragma once

#include <cstdint>

namespace bwg
{
	namespace elf
	{
		class ElfProgram
		{
		public:
			ElfProgram(uint64_t offset, uint64_t vaddr, uint64_t paddr, uint64_t filesz, uint64_t memsz, uint64_t align);
			virtual ~ElfProgram();

		private:
			uint64_t offset_;
			uint64_t vaddr_;
			uint64_t paddr_;
			uint64_t filesz_;
			uint64_t memsz_;
			uint64_t align_;
		};
	}
}
