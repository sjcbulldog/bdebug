#include "QuitCommand.h"
#include "Message.h"
#include "Debugger.h"

using namespace bwg::logfile;

namespace bwg
{
	namespace debug
	{
		QuitCommand::QuitCommand(Debugger* debug) : DebuggerCommand(debug)
		{
		}

		QuitCommand::~QuitCommand()
		{
		}

		bool QuitCommand::parseArgs(bool mi, const std::string& line, std::vector<CmdArg>& args)
		{
			(void)args;

			if (mi)
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "emachine interface parsing for 'quit' command is not implemented";
				logger() << msg;
				return false;
			}

			if (line.length() > 0)
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "extra characters after 'quit' command are not valid";
				logger() << msg;
				return false;
			}

			return true;
		}

		bool QuitCommand::exec(const std::string& keyword, std::vector<CmdArg>& args)
		{
			(void)keyword;
			(void)args;

			debugger()->running_ = false;
			return true;
		}
	}
}