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
			DebugBackend(bwg::logfile::Logger &logger) : logger_(logger) {
				symbols_ = nullptr;
			}

			virtual ~DebugBackend() {
			}

			void setSymbolProvider(ISymbolProvider* symbols) {
				symbols_ = symbols;
			}

			virtual bool reset() = 0;
			virtual bool reset(const std::string &mcutag) = 0;
			virtual bool run(const std::string &mcu) = 0;
			virtual bool stop(const std::string& mcu) = 0;
			virtual bool waitForStop(const std::string& mcu) = 0;
			virtual bool setBreakpoint(const std::string& mcutag, BreakpointType type, uint32_t addr, uint32_t size) = 0;

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
			virtual const MCUDesc& desc(const std::string& mcutag) const = 0;

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
					bwg::logfile::Message msg(bwg::logfile::Message::Type::Warning, "backend");
					msg << "search for symbol '" << name << "' with no symbol provider registered";
					logger_ << msg;
					return nullptr;
				}

				return symbols_->findSymbol(mcu, name);
			}

		protected:
			bwg::logfile::Logger& logger() {
				return logger_;
			}

			void setMCU(const std::string& mcutag, std::shared_ptr<BackendMCU> mcu) {
				mcus_.insert_or_assign(mcutag, mcu);
			}

			void setMCUDesc(const std::string& mcutag, const MCUDesc& desc) {
				desc_.insert_or_assign(mcutag, desc);
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
			bwg::logfile::Logger& logger_;
			ISymbolProvider* symbols_;
			std::map<std::string, std::shared_ptr<BackendMCU>> mcus_;
			std::map<std::string, MCUDesc> desc_;
		};
	}
}
