#pragma once

#include "Logger.h"
#include "DebugBackend.h"
#include <variant>
#include <string>
#include <vector>
#include <memory>

namespace bwg
{
	namespace debug
	{
		class Debugger;
		typedef std::variant<uint64_t, int64_t, std::string, double> CmdArg;

		class DebuggerCommand
		{
		public:
			DebuggerCommand(Debugger* debugger);
			virtual ~DebuggerCommand();

			virtual bool parseArgs(bool mi, const std::string& line, std::vector<CmdArg>& args) =  0;
			virtual bool exec(const std::string& line, std::vector<CmdArg>& args) = 0 ;

		protected:
			bwg::logfile::Logger& logger();
			std::shared_ptr<bwg::backend::DebugBackend> backend();
			Debugger* debugger() { return debugger_; }

			std::vector<CmdArg> toTokens(const std::string& line);
			size_t skipSpaces(const std::string& line, size_t index);

			bool parseUnsignedInteger(const std::string& token, uint64_t& value);
			std::string toString(const std::vector<CmdArg>& args);

		private:
			Debugger *debugger_;
		};
	}
}

