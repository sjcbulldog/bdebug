#include "RunCommand.h"
#include "Message.h"
#include "Debugger.h"

using namespace bwg::logfile;

namespace bwg
{
	namespace debug
	{
		RunCommand::RunCommand(Debugger* debug) : DebuggerCommand(debug)
		{
		}

		RunCommand::~RunCommand()
		{
		}

		bool RunCommand::parseArgs(bool mi, const std::string& line, std::vector<CmdArg>& args)
		{
			(void)args;

			if (mi)
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "emachine interface parsing for 'run' command is not implemented";
				logger() << msg;
				return false;
			}

			if (line.length() > 0)
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "extra characters after 'run' command are not valid";
				logger() << msg;
				return false;
			}

			return true;
		}

		bool RunCommand::exec(const std::string& keyword, std::vector<CmdArg>& args)
		{
			(void)keyword;
			(void)args;

			backend()->run();
			return true;
		}
	}
}