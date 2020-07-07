#include "BreakpointCommand.h"
#include "Message.h"
#include "Debugger.h"
#include <cassert>

using namespace bwg::logfile;
using namespace bwg::backend;

namespace bwg
{
	namespace debug
	{
		BreakpointCommand::BreakpointCommand(Debugger* debug) : DebuggerCommand(debug)
		{
		}

		BreakpointCommand::~BreakpointCommand()
		{
		}

		bool BreakpointCommand::parseArgs(bool mi, const std::string& line, std::vector<CmdArg>& args)
		{
			if (mi)
			{
				Message msg(Message::Type::Error, "cmdline");
				msg << "emachine interface parsing for 'break' command is not implemented";
				logger() << msg;
				return false;
			}

			args = toTokens(line);
			if (args.size() != 2)
			{
				args.clear();
				Message msg(Message::Type::Error, "cmdline");
				msg << "unexpected number of arguments, expected 2, got " << args.size();
				logger() << msg;
				return false;
			}

			if (!std::holds_alternative<std::string>(args[0]))
			{
				args.clear();
				Message msg(Message::Type::Error, "cmdline");
				msg << "unexpected argument type for first argument (MCU), expected a string";
				logger() << msg;
				return false;
			}

			if (!backend()->isMCUTagValid(std::get<std::string>(args[0])))
			{
				args.clear();
				Message msg(Message::Type::Error, "cmdline");
				msg << "value for argument type for first argument (MCU) is not valid, expected a value MCU tag:";
				for (const std::string& tag : backend()->mcuTags())
					msg << " " << tag;
				logger() << msg;
				return false;
			}

			if (std::holds_alternative<double>(args[1]))
			{
				args.clear();
				Message msg(Message::Type::Error, "cmdline");
				msg << "unexpected argument type for second argument (address), expected numeric address or symbol" ;
				logger() << msg;
				return false;
			}

			if (std::holds_alternative<int64_t>(args[1]))
			{
				args.clear();
				Message msg(Message::Type::Error, "cmdline");
				msg << "unexpected argument type for second argument (address), expected numeric address or symbol";
				logger() << msg;
				return false;
			}

			return true;
		}

		bool BreakpointCommand::exec(const std::string& keyword, std::vector<CmdArg>& args)
		{
			(void)keyword;

			Message msg(Message::Type::Debug, "command:break");
			msg << "Args: " << toString(args);
			logger() << msg;

			uint64_t addr = 0;
			uint64_t size = 2;
			if (std::holds_alternative<uint64_t>(args[1]))
			{
				addr = std::get<uint64_t>(args[1]);
			}
			else if (std::holds_alternative<std::string>(args[1]))
			{
				std::string mcu = std::get<std::string>(args[0]);
				std::string name = std::get<std::string>(args[1]);
				auto sym = debugger()->findSymbol(mcu, name);
				addr = sym->value();
			}

			backend()->setBreakpoint(std::get<std::string>(args[0]), BreakpointType::Hardware, addr, size);
			return true;
		}
	}
}