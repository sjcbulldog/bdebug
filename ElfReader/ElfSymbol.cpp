#include "ElfSymbol.h"
#include "MiscUtilsHex.h"
#include "ElfSection.h"

namespace bwg
{
	namespace elf
	{
		ElfSymbol::ElfSymbol(const std::string& name, uint64_t value, uint64_t size, ElfSymbol::Type type, std::shared_ptr<ElfSection> section, bool abs)
		{
			name_ = name;
			section_ = section;
			value_ = value;
			size_ = size;
			type_ = type;
			abs_ = abs;
		}

		ElfSymbol::~ElfSymbol()
		{
		}

		std::string ElfSymbol::toString() const
		{
			std::string ret;

			ret += name_;
			ret += ", section '" + section_->name() + "'";
			ret += ", type " + toString(type_);
			ret += ", value " + bwg::misc::MiscUtilsHex::n2hexstr<uint32_t>(static_cast<uint32_t>(value_));
			ret += ", size " + bwg::misc::MiscUtilsHex::n2hexstr<uint32_t>(static_cast<uint32_t>(size_));

			return ret;
		}
	}
}
