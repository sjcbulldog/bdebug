#pragma once

#include "Logger.h"
#include "DebugBackend.h"
#include "ISymbolProvider.h"
#include "ElfFile.h"
#include <filesystem>
#include <string>
#include <map>

namespace bwg
{
	namespace debug
	{
		class MCUWatcher;
		class DebuggerCommand;

		class Debugger : public bwg::backend::ISymbolProvider
		{
			friend class DebuggerCommand;
			friend class QuitCommand;

		public:
			Debugger(bwg::logfile::Logger& logger, std::shared_ptr<bwg::backend::DebugBackend> be);
			virtual ~Debugger();

			int run();
			bool loadElfFiles(std::map<std::string, std::filesystem::path>& elffiles);

			std::shared_ptr<const bwg::elf::ElfSymbol> findSymbol(const std::string& mcu, const std::string& name) override;

		private:
			void initializeCommands();
			bool connect();
			void runOneCommand();
			std::shared_ptr<DebuggerCommand> findCmdByKey(const std::string& key);

		private:
			//
			// For sending messages to the user
			//
			bwg::logfile::Logger& logger_;

			//
			// If true, we are connected to an IDE, us an abbreviated and easier to parse
			// set of messages
			//
			bool mi_;

			//
			// Watches all of the MCUs and reports any change of state to the user
			// 
			std::shared_ptr<MCUWatcher> watcher_;

			//
			// The debugger backend, does all of the heavy lifting
			//
			std::shared_ptr<bwg::backend::DebugBackend> backend_;

			//
			// The elf files loaded per MCU
			//
			std::map<std::string, std::shared_ptr<bwg::elf::ElfFile>> elffiles_;

			//
			// If true, we are running the command loop
			//
			bool running_;

			//
			// The propmt for the user before expecting a command
			//
			std::string prompt_;

			//
			// The set of commands the debugger understands
			//
			std::map<std::string, std::shared_ptr<DebuggerCommand>> commands_;
		};
	}
}

