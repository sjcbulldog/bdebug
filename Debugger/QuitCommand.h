#pragma once

#include "DebuggerCommand.h"

namespace bwg
{
	namespace debug
	{
		class QuitCommand : public DebuggerCommand
		{
		public:
			QuitCommand(Debugger* debug);
			virtual ~QuitCommand();

			bool parseArgs(bool mi, const std::string& line, std::vector<CmdArg>& args) override ;
			bool exec(const std::string& line, std::vector<CmdArg>& args) override ;
		};
	}
}


