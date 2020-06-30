#include "Debugger.h"
#include "MicroController.h"
#include <thread>
#include <iostream>

using namespace bwg::logfile;
using namespace bwg::backend;

namespace bwg
{
	namespace debug
	{
		Debugger::Debugger(Logger& logger, std::shared_ptr<DebugBackend> be, 
			std::map<std::string, std::filesystem::path> &elffiles) : logger_(logger)
		{
			backend_ = be;
			elffiles_ = elffiles;
		}

		Debugger::~Debugger()
		{
		}

		std::shared_ptr<const bwg::elf::ElfSymbol> Debugger::findSymbol(const std::string& mcu, const std::string& name)
		{
			auto it = mcus_.find(mcu);
			if (it == mcus_.end())
				return nullptr;

			return it->second->elffile().findSymbol(name);
		}

		int Debugger::run()
		{
			int ret = 0;

			//
			// Connect to the debug hardware and figure out what we are connected to.  Nothing
			// in the backend should need symbols here as they are not yet available
			//
			if (!connect())
				return 1;

			//
			// Create the MCU devices and read any ELF files associated
			//
			if (!createMCUs())
				return 1;

			//
			// Setup the backend to get symbols for the various MCUs
			//
			backend_->setSymbolProvider(this);

			//
			// Now have the backend do phase two initialization, which is anything that 
			// requires symbols from the elf file
			//
			if (!backend_->initPhaseTwo())
				return 1;

			//
			// Run the CM4 to main
			//
			backend_->reset("cm0p");
			// backend_->run("cm0p");

			std::string line;
			while (true) 
			{
				std::cout << "BDG> ";
				std::cout.flush();
				std::getline(std::cin, line);
				if (line.length() >= 4 && line.substr(0, 4) == "quit")
					break;
			}

			return ret;
		}

		bool Debugger::connect()
		{
			return backend_->connect();
		}

		bool Debugger::createMCUs()
		{
			std::list<std::string> mcutags = backend_->mcuTags();

			for (const std::string& mcutag : mcutags)
			{
				auto it = elffiles_.find(mcutag);
				if (it != elffiles_.end())
				{
					//
					// We have an elf file for this MCU
					//
					auto mcu = std::make_shared<MicroController>(this, mcutag, backend_->desc(mcutag));
					if (!mcu->loadElfFile(it->second))
						return false;

					mcus_.insert_or_assign(mcutag, mcu);
				}
			}

			return true;
		}
	}
}
