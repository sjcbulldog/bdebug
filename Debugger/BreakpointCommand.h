#pragma once

#include "DebuggerCommand.h"

namespace bwg
{
	namespace debug
	{
		class BreakpointCommand : public DebuggerCommand
		{
		public:
			BreakpointCommand(Debugger*);
			virtual ~BreakpointCommand();

			bool parseArgs(bool mi, const std::string& line, std::vector<CmdArg>& args) override;
			bool exec(const std::string& line, std::vector<CmdArg>& args) override;

		private:
		};
	}
}
