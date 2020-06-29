#include "ElfFile.h"
#include "ElfSection.h"
#include "ElfSymbol.h"

namespace bwg
{
	namespace elf
	{
		ElfFile::ElfFile()
		{
		}

		ElfFile::~ElfFile()
		{
		}

		void ElfFile::addSection(std::shared_ptr<ElfSection> sect)
		{
			sections_.push_back(sect);
		}

		void ElfFile::addSymbol(std::shared_ptr<ElfSymbol> symbol)
		{
			symbols_.push_back(symbol);
		}

		std::shared_ptr<const ElfSymbol> ElfFile::findSymbol(const std::string& name) const
		{
			auto list = findSymbols(name);
			if (list.size() == 1)
				return list.front();

			return nullptr;
		}

		std::list<std::shared_ptr<const ElfSymbol>> ElfFile::findSymbols(const std::string& name) const
		{
			std::list<std::shared_ptr<const ElfSymbol>> symlist;

			for (auto sym : symbols_)
			{
				if (sym->name() == name)
					symlist.push_back(sym);
			}

			return symlist;
		}
	}
}
