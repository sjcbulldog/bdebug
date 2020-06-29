#pragma once

#include <string>
#include <memory>

namespace bwg
{
	namespace elf
	{
		class ElfSection;

		class ElfSymbol
		{
		public:
			enum class Type
			{
				None,
				Object,
				Function,
				Section,
				File,
				Common,
				TLS,
				Unknown
			};
		public:
			ElfSymbol(const std::string& name, uint64_t value, uint64_t size, Type type, std::shared_ptr<ElfSection> section, bool abs);
			virtual ~ElfSymbol();

			bool isAbsolute() const { return abs_; }
			std::string toString() const;

			const std::string& name() const {
				return name_;
			}

			uint64_t value() const {
				return value_;
			}

			uint64_t size() const {
				return size_;
			}

			static std::string toString(Type t)
			{
				std::string ret;

				switch (t)
				{
				case Type::None:
					ret = "None";
					break;
				case Type::Object:
					ret = "Object";
					break;
				case Type::Function:
					ret = "Function";
					break;
				case Type::Section:
					ret = "Section";
					break;
				case Type::File:
					ret = "File";
					break;
				case Type::Common:
					ret = "Common";
					break;
				case Type::TLS:
					ret = "TLS";
					break;
				default:
					ret = "Unknown";
					break;
				}

				return ret;
			}

		private:
			std::string name_;
			std::shared_ptr<ElfSection> section_;
			uint64_t value_;
			uint64_t size_;
			Type type_;
			bool abs_;
		};
	}
}
