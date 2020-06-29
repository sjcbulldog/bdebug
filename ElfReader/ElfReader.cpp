#include "ElfReader.h"
#include "ElfFile.h"
#include "ElfSection.h"
#include "ElfProgram.h"
#include "ElfSymbol.h"
#include "elffmt.h"
#include <Logger.h>
#include <Message.h>
#include <fstream>
#include <iostream>

using namespace bwg::logger;

namespace bwg
{
	namespace elf
	{
		ElfReader::ElfReader(ElfFile& efile, const std::filesystem::path& filename, Logger &logger) : efile_(efile), logger_(logger)
		{
			filename_ = filename;
		}

		ElfReader::~ElfReader()
		{
		}

		ElfSymbol::Type ElfReader::intToType(uint8_t inttype)
		{
			ElfSymbol::Type ret = ElfSymbol::Type::Unknown;

			switch (inttype)
			{
			case ELFSYM_STT_NOTYPE:
				ret = ElfSymbol::Type::None;
				break;
			case ELFSYM_STT_OBJECT:
				ret = ElfSymbol::Type::Object;
				break;
			case ELFSYM_STT_FUNC:
				ret = ElfSymbol::Type::Function;
				break;
			case ELFSYM_STT_SECTION:
				ret = ElfSymbol::Type::Section;
				break;
			case ELFSYM_STT_FILE:
				ret = ElfSymbol::Type::File;
				break;
			case ELFSYM_STT_COMMON:
				ret = ElfSymbol::Type::Common;
				break;
			case ELFSYM_STT_TLS:
				ret = ElfSymbol::Type::TLS;
				break;
			}

			return ret;
		}

		bool ElfReader::read()
		{
			std::ifstream infile(filename_, std::ifstream::binary);
			if (!infile.is_open())
			{
				Message msg(Message::Type::Error, "eflreader");
				msg << "cannot open file '" << filename_ << "' for reading";
				logger_ << msg;
				return false;
			}

			infile.seekg(0, std::ios_base::end);
			if (infile.bad() || infile.fail())
			{
				Message msg(Message::Type::Error, "eflreader");
				msg << "bdebug: error: cannot determine file '" << filename_ << "' size";
				logger_ << msg;
				return false;
			}
			filesize_ = infile.tellg();

			infile.seekg(0, std::ios_base::beg);
			if (infile.bad() || infile.fail())
			{\
				Message msg(Message::Type::Error, "eflreader");
				msg << "bdebug: error: cannot determine file '" << filename_ << "' size";
				return false;
			}

			if (!readHeader(infile))
				return false;

			if (!readProgramHeaders(infile))
				return false;

			if (!readSections(infile))
				return false;

			return true;
		}

		bool ElfReader::readHeader(std::ifstream& infile)
		{
			infile.read((char*)&header_, sizeof(header_));
			if (infile.fail() && infile.eof())
			{
				Message msg(Message::Type::Error, "eflreader");
				msg << "bdebug: error: file '" << filename_ << "' does not contain a valid ELF header (EOF reading header)";
				logger_ << msg;
				return false;
			}

			if (header_.e_ident_[0] != ELF_MAGIC_BYTE0 || header_.e_ident_[1] != ELF_MAGIC_BYTE1 ||
				header_.e_ident_[2] != ELF_MAGIC_BYTE2 || header_.e_ident_[3] != ELF_MAGIC_BYTE3)
			{
				Message msg(Message::Type::Error, "eflreader");
				msg << "file '" << filename_ << "' does not contain a valid ELF image (bad header signature)";
				logger_ << msg;
				return false;
			}

			if (header_.e_machine_ != ELFHDR_MACHINE_ARM)
			{
				Message msg(Message::Type::Error, "eflreader");
				msg << "file '" << filename_ << "' does not contain an ARM images, this debugger only supports ARM devices";
				logger_ << msg;
				return false;
			}

			efile_.setResetVector(header_.e_entry_);

			return true;
		}

		bool ElfReader::readProgramHeaders(std::ifstream& infile)
		{
			elfpgmheader pgmhdr;

			if (header_.e_phoff_ > filesize_)
			{
				Message msg(Message::Type::Error, "eflreader");
				msg << "file '" << filename_ << "' does not contain a valid ELF header (invalid program header table location)";
				logger_ << msg;
				return false;
			}

			infile.seekg(header_.e_phoff_, std::ios_base::beg);
			if (infile.bad() || infile.fail())
			{
				Message msg(Message::Type::Error, "eflreader");
				msg << "file '" << filename_ << "' does not contain a valid ELF header (cannot seek to program headeer)";
				logger_ << msg;
				return false;
			}

			for (int i = 0; i < header_.e_phnum_; i++)
			{
				infile.read((char*)&pgmhdr, sizeof(pgmhdr));
				if (infile.fail() && infile.eof())
				{
					Message msg(Message::Type::Error, "eflreader");
					msg << "file '" << filename_ << "' does not contain valid ELF content (EOF reading program header)";
					logger_ << msg;
					return false;
				}

				if (pgmhdr.p_type_ == ELFPGM_PT_LOAD)
				{
					auto pgm = std::make_shared<ElfProgram>(pgmhdr.p_offset_, pgmhdr.p_vaddr_, pgmhdr.p_paddr_, pgmhdr.p_filesz_, pgmhdr.p_memsz_, pgmhdr.p_align_);
					programs_.push_back(pgm);
				}
			}
			return true;
		}

		bool ElfReader::readSections(std::ifstream& infile)
		{
			elfsectheader secthdr;
			bool st = true;

			if (header_.e_shoff_ > filesize_)
			{
				Message msg(Message::Type::Error, "eflreader");
				msg << "file '" << filename_ << "' does not contain a valid ELF header (invalid section header table location)";
				logger_ << msg;
				return false;
			}

			infile.seekg(header_.e_shoff_, std::ios_base::beg);
			if (infile.bad() || infile.fail())
			{
				Message msg(Message::Type::Error, "eflreader");
				msg << "file '" << filename_ << "' does not contain a valid ELF header (cannot seek to program headeer)";
				logger_ << msg;
				return false;
			}

			for (int i = 0; st && i < header_.e_shnum_; i++)
			{
				infile.read((char*)&secthdr, sizeof(secthdr));
				if (infile.fail() && infile.eof())
				{
					Message msg(Message::Type::Error, "eflreader");
					msg << "file '" << filename_ << "' does not contain valid ELF content (EOF reading program header)";
					logger_ << msg;
					return false;
				}

				sections_.push_back(secthdr);

				if (secthdr.sh_type_ == ELFSECT_SHT_STRTAB && i == header_.e_shstrndx_)
				{
					st = readStringTable(infile, section_strings_, secthdr);
				}
			}

			size_t secno = findSectionIndex(".strtab");
			if (secno < sections_.size())
			{
				st = readStringTable(infile, strtab_strings_, sections_[secno]);
			}

			secno = findSectionIndex(".stabstr");
			if (secno < sections_.size())
			{
				st = readStringTable(infile, stabstr_strings_, sections_[secno]);
			}

			for (size_t i = 0; i < sections_.size(); i++)
			{
				const elfsectheader& sec = sections_[i];
				if (sec.sh_type_ == ELFSECT_SHT_PROGBITS || sec.sh_type_ == ELFSECT_SHT_NOBITS || 
					sec.sh_type_ == ELFSECT_SHT_ARM_ATTRIBUTES || sec.sh_type_ == ELFSECT_SHT_ARM_EXIDX)
				{
					std::string name = (const char*)&section_strings_[sec.sh_name_];
					auto secptr = std::make_shared<ElfSection>(name);
					section_ptrs_.insert_or_assign(i, secptr);

					efile_.addSection(secptr);
				}
			}

			secno = findSectionIndex(".symtab");
			if (secno < sections_.size() && sections_[secno].sh_type_ == ELFSECT_SHT_SYMTAB)
			{
				st = readSymbolTable(infile, sections_[secno]);
			}

			return true;
		}

		bool ElfReader::readSymbolTable(std::ifstream& infile, const elfsectheader& sect)
		{
			elfsymbol sym;

			std::ifstream::pos_type pos = infile.tellg();

			infile.seekg(sect.sh_offset_);
			if (infile.bad() || infile.fail())
			{
				Message msg(Message::Type::Error, "eflreader");
				msg << "file '" << filename_ << "' does not contain a valid ELF header (cannot seek to string table)";
				logger_ << msg;
				return false;
			}

			while (static_cast<uint64_t>(infile.tellg()) + sizeof(elfsymbol) < sect.sh_offset_ + sect.sh_size_)
			{
				infile.read((char*)&sym, sizeof(sym));
				if (infile.fail() && infile.eof())
				{
					Message msg(Message::Type::Error, "eflreader");
					msg << "file '" << filename_ << "' is not a valid ELF file (error reading symbol table)";
					logger_ << msg;
					return false;
				}

				// const std::string& name, uint64_t value, uint64_t size, Type type, std::shared_ptr<ElfSection> section;

				std::string name = (const char*)&strtab_strings_[sym.st_name_];
				ElfSymbol::Type type = intToType(ELFSYM_ST_TYPE(sym.st_info_));

				std::shared_ptr<ElfSection> sectptr;
				bool abs = false;

				if (sym.st_shndx_ == 0x00)
				{
					abs = false;
				}
				else if (sym.st_shndx_ == 0xFFF1)
				{
					abs = true;
				}
				else
				{
					auto it = section_ptrs_.find(sym.st_shndx_);
					if (it == section_ptrs_.end())
					{
						Message msg(Message::Type::Warning, "eflreader");
						msg << "section " << sym.st_shndx_ << " is missing for symbol '" << name << "'";
						logger_ << msg;
					}
					else
					{
						sectptr = it->second;
					}
				}

				auto symptr = std::make_shared<ElfSymbol>(name, sym.st_value_, sym.st_size_, type, sectptr, abs);
				efile_.addSymbol(symptr);
			}

			infile.seekg(pos);
			return true;
		}

		bool ElfReader::readStringTable(std::ifstream& infile, std::vector<uint8_t> &storage, const elfsectheader& sect)
		{
			std::ifstream::pos_type pos = infile.tellg();
			storage.resize(sect.sh_size_);

			infile.seekg(sect.sh_offset_);
			if (infile.bad() || infile.fail())
			{
				Message msg(Message::Type::Error, "eflreader");
				msg << "file '" << filename_ << "' does not contain a valid ELF header (cannot seek to string table)";
				logger_ << msg;
				return false;
			}

			infile.read((char*)&storage[0], storage.size());
			if (infile.fail() && infile.eof())
			{
				Message msg(Message::Type::Error, "eflreader");
				msg << "file '" << filename_ << "' does not contain valid ELF content (error reading string table)";
				logger_ << msg;
				return false;
			}

			infile.seekg(pos);
			return true;
		}

		size_t ElfReader::findSectionIndex(const std::string& name)
		{
			for (size_t i = 0; i < sections_.size(); i++)
			{
				if (name == (const char *)&section_strings_[sections_[i].sh_name_])
					return i;
			}

			return std::numeric_limits<size_t>::max();
		}
	}
}
