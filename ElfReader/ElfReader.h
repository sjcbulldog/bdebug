#pragma once

#include "elffmt.h"
#include "ElfSymbol.h"
#include <Logger.h>
#include <filesystem>
#include <string>
#include <fstream>
#include <vector>
#include <map>

namespace bwg
{
	namespace elf
	{
		class ElfFile;
		class ElfSection;
		class ElfProgram;

		class ElfReader
		{
		public:
			ElfReader(ElfFile& efile, const std::filesystem::path& filename, bwg::logfile::Logger& logger);
			virtual ~ElfReader();

			bool read();

		private:
			bool readHeader(std::ifstream &infile);
			bool readProgramHeaders(std::ifstream& infile);
			bool readSections(std::ifstream& infile);
			bool readStringTable(std::ifstream& infile, std::vector<uint8_t>& storage, const elfsectheader& hdr);
			bool readSymbolTable(std::ifstream& infile, const elfsectheader& hdr);

			size_t findSectionIndex(const std::string& name);

			ElfSymbol::Type intToType(uint8_t value);

		private:
			std::filesystem::path filename_;
			std::ifstream::pos_type filesize_;
			elfheader header_;
			std::vector<std::shared_ptr<ElfProgram>> programs_;
			std::vector<elfsectheader> sections_;
			std::vector<uint8_t> section_strings_;
			std::vector<uint8_t> strtab_strings_;
			std::vector<uint8_t> stabstr_strings_;
			std::map<size_t, std::shared_ptr<ElfSection>> section_ptrs_;

			ElfFile& efile_;
			bwg::logfile::Logger& logger_;
		};
	}
}


