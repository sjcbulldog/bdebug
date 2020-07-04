#pragma once

#include "Logger.h"
#include "DebugBackend.h"
#include "ISymbolProvider.h"
#include "ElfFile.h"
#include "UserInputOutputDevice.h"
#include <filesystem>
#include <string>
#include <map>

namespace bwg
{
	namespace debug
	{
		class Watcher;

		class Debugger : public bwg::backend::ISymbolProvider
		{
			friend class DebuggerCommand;

		public:
			Debugger(bwg::logfile::Logger& logger, std::shared_ptr<bwg::backend::DebugBackend> be);
			virtual ~Debugger();

			int run();
			bool loadElfFiles(std::map<std::string, std::filesystem::path>& elffiles);

		protected:
			std::shared_ptr<const bwg::elf::ElfSymbol> findSymbol(const std::string& mcu, const std::string& name) override;
			std::shared_ptr<DebugBackend> backend() {
				return backend_;
			}

		private:
			enum class State {

			};

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

		private:
			bwg::logfile::Logger& logger_;

			UserInputOutputDevice input_output_;
			std::shared_ptr<Watcher> watcher_;

			std::shared_ptr<bwg::backend::DebugBackend> backend_;
			std::map<std::string, std::shared_ptr<bwg::elf::ElfFile>> elffiles_;

			std::string prompt_;
			bool initialized_;
		};
	}
}

