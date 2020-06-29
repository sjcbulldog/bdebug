#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>
#include <list>

namespace bwg
{
	namespace elf
	{
		class ElfSymbol;
		class ElfSection;

		class ElfFile
		{
		public:
			ElfFile();
			virtual ~ElfFile();

			void setResetVector(uint32_t v) {
				reset_vector_ = v;
			}

			void addSection(std::shared_ptr<ElfSection> sect);
			void addSymbol(std::shared_ptr<ElfSymbol> symbol);

			std::shared_ptr<const ElfSymbol> findSymbol(const std::string& name) const;
			std::list<std::shared_ptr<const ElfSymbol>> findSymbols(const std::string& name) const;

		private:
			uint64_t reset_vector_;
			std::vector<std::shared_ptr<ElfSection>> sections_;
			std::vector<std::shared_ptr<ElfSymbol>> symbols_;
		};
	}
}
