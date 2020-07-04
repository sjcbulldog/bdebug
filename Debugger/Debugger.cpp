#include "Debugger.h"
#include "ElfReader.h"
#include "UIOMessageDestination.h"
#include "MCUWatcher.h"
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
			initialized_ = false;

			logger.clearDestinations();
			auto dest = std::make_shared<UIOMessageDestination>(input_output_);
			logger.addDestination(dest);
		}

		Debugger::~Debugger()
		{
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

			watcher_ = std::make_shared<Watcher>();

			//
			// Now prompt the user
			//
			std::string line;
			while (true) 
			{
				if (!input_.getLine(line))
					break;


				if (line.length() >= 4 && line.substr(0, 4) == "quit")
					break;
			}

			return ret;
		}

		bool Debugger::connect()
		{
			return backend_->connect();
		}
	}
}
