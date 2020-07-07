#include "Debugger.h"
#include "ElfReader.h"
#include "MCUWatcher.h"

#include "QuitCommand.h"
#include "RunCommand.h"
#include "BreakpointCommand.h"

#include <thread>
#include <iostream>

using namespace bwg::logfile;
using namespace bwg::backend;
using namespace bwg::elf;

namespace bwg
{
	namespace debug
	{
		Debugger::Debugger(Logger& logger, std::shared_ptr<DebugBackend> be) : logger_(logger)
		{
			backend_ = be;
			prompt_ = "BDebug> ";

			initializeCommands();

			running_ = true;
		}

		Debugger::~Debugger()
		{
			if (watcher_ != nullptr)
				watcher_->stop();
		}

		void Debugger::initializeCommands()
		{
			std::shared_ptr<DebuggerCommand> cmd;

			commands_["quit"] = std::make_shared<QuitCommand>(this);
			commands_["run"] = std::make_shared<RunCommand>(this);
			commands_["break"] = std::make_shared<BreakpointCommand>(this);
		}

		bool Debugger::loadElfFiles(std::map<std::string, std::filesystem::path>& elffiles)
		{
			for (auto pair : elffiles)
			{
				auto file = std::make_shared<ElfFile>();
				ElfReader rdr(*file, pair.second, logger_);

				if (!rdr.read())
					return false;

				elffiles_[pair.first] = file;
			}

			backend_->setSymbolProvider(this);

			return true;
		}

		std::shared_ptr<const bwg::elf::ElfSymbol> Debugger::findSymbol(const std::string& mcu, const std::string& name)
		{
			auto it = elffiles_.find(mcu);
			if (it == elffiles_.end())
				return nullptr;

			return it->second->findSymbol(name);
		}

		int Debugger::run()
		{
			int ret = 0;

			//
			// Connect to the debug hardware and figure out what we are connected to.  This should
			// create the MCU threads, one per MCU monitoring the traffic with the backend.
			//
			if (!connect())
				return 1;

			//
			// Wait for the backend to be ready
			//
			while (!backend_->ready())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
			}

			Message msg(Message::Type::Debug, "debugger");
			msg << "all threads ready";
			logger_ << msg;

			backend_->reset();

			watcher_ = std::make_shared<MCUWatcher>(backend_);

			//
			// Now prompt the user
			//
			std::string line;
			while (running_) 
			{
				runOneCommand();
			}

			return ret;
		}

		bool Debugger::connect()
		{
			return backend_->connect();
		}

		std::shared_ptr<DebuggerCommand> Debugger::findCmdByKey(const std::string& key) 
		{
			auto it = commands_.find(key);
			if (it == commands_.end())
				return nullptr;

			return it->second;
		}

		void Debugger::runOneCommand()
		{
			std::string line;

			if (std::cin.bad() || std::cin.eof() || std::cin.fail())
			{
				running_ = false;
				return;
			}

			std::cout << prompt_;
			std::cout.flush();

			if (!std::getline(std::cin, line))
			{
				running_ = false;
				return;
			}

			std::string key, args;

			size_t pos = line.find(' ');
			if (pos == std::string::npos)
			{
				key = line;
			}
			else
			{
				key = line.substr(0, pos);
				args = line.substr(pos);
			}

			if (key.length() == 0)
				return;

			std::shared_ptr<DebuggerCommand> cmd = findCmdByKey(key);
			if (cmd == nullptr)
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "unknown command '" << key << "'";
				logger_ << msg;
			}
			else
			{
				bool parsed = false;
				std::vector<CmdArg> argvec;

				parsed = cmd->parseArgs(mi_, args, argvec);
				if (parsed)
					cmd->exec(key, argvec);
			}
		}
	}
}
