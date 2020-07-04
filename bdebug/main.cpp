#include <iostream>
#include "Logger.h"
#include "OStreamDestination.h"
#include "GDBServerBackend.h"
#include "Debugger.h"
#include "ElfReader.h"
#include "ElfFile.h"
#include <map>

using namespace bwg::elf;
using namespace bwg::logfile;
using namespace bwg::backend;
using namespace bwg::debug;

int main(int ac, char** av)
{
	int ret = 0;
	std::string configfile;
	std::string logfile;
	std::string openocd;
	std::map<std::string, std::filesystem::path> elffiles;

	Logger logger;
	logger.addDestination(std::make_shared<OStreamDestination>(std::cerr));

	ac--;
	av++;

	while (ret == 0 && ac-- > 0)
	{
		std::string arg = *av++;

		if (arg == "--config")
		{
			if (ac == 0)
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "--config command line arguments requires a filename";
				logger << msg;
				ret = 1;
			}
			configfile = *av++;
			ac--;
		}
		else if (arg == "--logfile")
		{
			if (ac == 0)
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "--logfile command line arguments requires a filename";
				logger << msg;
				ret = 1;
			}
			logfile = *av++;
			ac--;
		}
		else if (arg == "--openocd")
		{
			if (ac == 0)
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "--openocd command line arguments requires a filename";
				logger << msg;
				ret = 1;
			}
			openocd = *av++;
			ac--;
		}
		else if (arg == "--module")
		{
			if (ac == 0)
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "--module command line arguments requires a module name";
				logger << msg;
				ret = 1;
			}
			logger.enable(*av++);
			ac--;
		}
		else if (arg == "--debug")
		{
			logger.enable(Message::Type::Debug);
		}
		else if (arg == "--elffile")
		{
			if (ac <= 1)
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "--elffile command line arguments requires two following arguments";
				logger << msg;
				ret = 1;
			}
			std::string mcutag = *av++;
			std::filesystem::path filename = *av++;
			ac--;
			ac--;

			if (!std::filesystem::exists(filename))
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "--elffile argument, elf file '" << filename << "' does not exists";
				logger << msg;
				ret = 1;
			}

			if (!std::filesystem::is_regular_file(filename))
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "--elffile argument, elf file '" << filename << "' exists but is not a file";
				logger << msg;
				ret = 1;
			}

			elffiles.insert_or_assign(mcutag, filename);
		}
		else
		{
			Message msg(Message::Type::Error, "cmdline");
			msg << "unknown command argument '" << arg << "'";
			logger << msg;
			ret = 1;
		}
	}

	if (ret != 0)
		return ret;

	auto be = std::make_shared<GDBServerBackend>(logger, configfile);
	if (openocd.length() > 0)
		be->setExePath(openocd);

	Debugger debugger(logger, be);
	if (!debugger.loadElfFiles(elffiles))
	{
		Message msg(Message::Type::Error, "cmdline");
		msg << "failure loading ELF files ";
		logger << msg;
		return 1;
	}

	ret = debugger.run();
	return ret;
}