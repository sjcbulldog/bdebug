#pragma once

#include <variant>
#include <string>
#include <vector>
#include <memory>

namespace bwg
{
	namespace debugger
	{
		class Debugger;
		typedef std::variant<uint64_t, std::string, double> CmdArg;

		class DebuggerCommand
		{
		public:
			DebuggerCommand(d::shared_ptr<Debugger> debugger) {
				debugger_ = debugger;
			}

			virtual ~DebuggerCommand() {
			}

			virtual bool parseHuman(const std::string& line, std::vector<CmdArg>& args);
			virtual bool parseMachineInterface(const std::string& line, std::vector<CmdArg>& args);
			virtual bool exec(const std::string& line, std::vector<CmdArg>& args);

		private:
			std::shared_ptr<Debugger> debugger_;

		};
	}
}

