#pragma once

#include "ElfSymbol.h"

namespace bwg
{
	namespace backend
	{
		class ISymbolProvider
		{
		public:
			virtual std::shared_ptr<const bwg::elf::ElfSymbol> findSymbol(const std::string& mcu, const std::string& name) = 0;
		};
	}
}