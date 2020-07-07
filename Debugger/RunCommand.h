#pragma once

#include "DebuggerCommand.h"

namespace bwg
{
	namespace debug
	{
		class RunCommand : public DebuggerCommand
		{
		public:
			RunCommand(Debugger*);
			virtual ~RunCommand();

			bool parseArgs(bool mi, const std::string& line, std::vector<CmdArg>& args) override;
			bool exec(const std::string& line, std::vector<CmdArg>& args) override;

		private:
		};
	}
}
