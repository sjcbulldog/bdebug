#pragma once

#include "BackendMCU.h"
#include "ISymbolProvider.h"
#include "Logger.h"
#include "Message.h"
#include <memory>
#include <map>

namespace bwg
{
	namespace backend
	{
		class MCUDesc;

		class DebugBackend
		{
		public:
			DebugBackend(bwg::logger::Logger &logger) : logger_(logger) {
				symbols_ = nullptr;
			}

			virtual ~DebugBackend() {
			}

			void setSymbolProvider(ISymbolProvider* symbols) {
				symbols_ = symbols;
			}

			void setStop(const std::string &tag, bool reset, bool main) {
				auto it = mcus_.find(tag);
				if (it != mcus_.end())
				{
					it->second->setStop(reset, main);
				}
			}


			virtual bool restart() = 0;
			virtual bool run(const std::string &mcu, bool wait = true) = 0;

			//
			// Connect to the target
			//
			virtual bool connect() = 0;

			//
			// Return the sets of MCUs on the target device
			//
			virtual std::list<std::string> mcuTags() = 0;

			//
			// Return the descriptor for an MCU attached to the given descriptor
			//
			virtual const MCUDesc& desc(const std::string& tag) const = 0;

			//
			// Do any initialization that is required after the ELF file symbols have been loaded
			//
			virtual bool initPhaseTwo() = 0;

			//
			// Find a symbol for the requested MCU by asking the symbol provider
			//
			std::shared_ptr<const bwg::elf::ElfSymbol> findSymbol(const std::string& mcu, const std::string& name) {
				if (symbols_ == nullptr)
				{
					bwg::logger::Message msg(bwg::logger::Message::Type::Warning, "backend");
					msg << "search for symbol '" << name << "' with no symbol provider registered";
					logger_ << msg;
					return nullptr;
				}

				return symbols_->findSymbol(mcu, name);
			}

		protected:
			bwg::logger::Logger& logger() {
				return logger_;
			}

			void setMCU(const std::string& tag, std::shared_ptr<BackendMCU> mcu) {
				mcus_.insert_or_assign(tag, mcu);
			}

			void setMCUDesc(const std::string& tag, const MCUDesc& desc) {
				desc_.insert_or_assign(tag, desc);
			}

			std::map<std::string, std::shared_ptr<BackendMCU>>& mcus() {
				return mcus_;
			}

			std::map<std::string, MCUDesc>& descs() {
				return desc_;
			}

			const std::map<std::string, MCUDesc>& descs() const {
				return desc_;
			}

		private:
			bwg::logger::Logger& logger_;
			ISymbolProvider* symbols_;
			std::map<std::string, std::shared_ptr<BackendMCU>> mcus_;
			std::map<std::string, MCUDesc> desc_;
		};
	}
}
