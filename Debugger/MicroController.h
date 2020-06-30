#pragma once

#include "MCUDesc.h"
#include "ElfFile.h"
#include <string>
#include <filesystem>

namespace bwg
{
	namespace debug
	{
		class Debugger;

		class MicroController
		{
		public:
			MicroController(Debugger *debug, const std::string& mcutag, const bwg::backend::MCUDesc& desc);
			virtual ~MicroController();

			bool loadElfFile(const std::filesystem::path &elfFile);
			const bwg::elf::ElfFile& elffile() const {
				return elffile_;
			}

		private:
			Debugger* debug_;
			std::string mcutag_;
			const bwg::backend::MCUDesc& desc_;
			bwg::elf::ElfFile elffile_;
		};
	}
}
