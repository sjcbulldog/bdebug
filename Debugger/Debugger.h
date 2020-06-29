#pragma once

#include "Logger.h"
#include "DebugBackend.h"
#include "ISymbolProvider.h"
#include <filesystem>
#include <string>
#include <map>

namespace bwg
{
	namespace debug
	{
		class MicroController;

		class Debugger : public bwg::backend::ISymbolProvider
		{
			friend class MicroController;

		public:
			Debugger(bwg::logfile::Logger& logger, std::shared_ptr<bwg::backend::DebugBackend> be, 
					std::map<std::string, std::filesystem::path> &elffiles);
			virtual ~Debugger();

			int run();

			std::shared_ptr<const bwg::elf::ElfSymbol> findSymbol(const std::string& mcu, const std::string& name) override;

		protected:

		private:
			//
			// These are shared with MicroController.  While C++ wouild allows MicroController to have
			// access to everyting since it is a friend, we limit it to methods in this private section
			//
			bwg::logfile::Logger& logger() {
				return logger_;
			}

		private:
			bool connect();
			bool createMCUs();
			bool loadElfFiles();

		private:
			bwg::logfile::Logger& logger_;
			std::shared_ptr<bwg::backend::DebugBackend> backend_;
			std::map<std::string, std::filesystem::path> elffiles_;
			std::map<std::string, std::shared_ptr<MicroController>> mcus_;
		};
	}
}

