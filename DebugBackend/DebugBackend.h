#pragma once

#include "BackendMCU.h"
#include "ISymbolProvider.h"
#include "Logger.h"
#include "Message.h"
#include <memory>
#include <map>
#include <mutex>

namespace bwg
{
	namespace backend
	{
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

			void setAllReset() {
				for (auto pair : mcus_)
				{
					pair.second->setState(BackendMCU::MCUState::Reset);
				}
			}

			virtual bool connect() = 0;
			virtual bool ready() = 0;
			virtual bool reset() = 0;
			virtual bool run() = 0;
			virtual bool run(const std::string &mcu) = 0;
			virtual bool stop() = 0 ;
			virtual bool stop(const std::string& mcu) = 0;
			virtual bool setBreakpoint(const std::string& mcutag, BreakpointType type, uint64_t addr, uint64_t size) = 0;
			virtual bool removeBreakpoint(const std::string& mcutag, BreakpointType type, uint64_t addr, uint64_t size) = 0;

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

			bwg::logfile::Logger& logger() {
				return logger_;
			}

			std::list<std::shared_ptr<BackendMCU>> mcus() {
				std::list<std::shared_ptr<BackendMCU>> mculist;

				for (auto pair : mcus_)
					mculist.push_back(pair.second);

				return mculist;
			}

			std::list<std::string> mcuTags() {
				std::list<std::string> ret;

				for (auto pair : mcus_)
					ret.push_back(pair.first);

					return ret;
			}

			bool isMCUTagValid(const std::string& mcutag) {
				auto it = mcus_.find(mcutag);
					return it != mcus_.end();
			}

			std::shared_ptr<BackendMCU> getMCUByTag(const std::string& mcutag) {
				auto it = mcus_.find(mcutag);
				if (it == mcus_.end())
					return nullptr;

				return it->second;
			}

		protected:
			void setMCU(const std::string& mcutag, std::shared_ptr<BackendMCU> mcu) {
				std::lock_guard<std::mutex> guard(backend_lock_);
				mcus_.insert_or_assign(mcutag, mcu);
			}

		private:
			bwg::logfile::Logger& logger_;
			ISymbolProvider* symbols_;
			std::map<std::string, std::shared_ptr<BackendMCU>> mcus_;
			std::mutex backend_lock_;
		};
	}
}
