#include "DebuggerCommand.h"
#include "Debugger.h"
#include "MiscUtilsHex.h"
#include <cassert>

namespace bwg
{
	namespace debug
	{
		DebuggerCommand::DebuggerCommand(Debugger* debug) 
		{
			debugger_ = debug;
		}

		DebuggerCommand::~DebuggerCommand()
		{

		}
		bwg::logfile::Logger& DebuggerCommand::logger() 
		{
			return debugger_->logger_;
		}

		std::shared_ptr<bwg::backend::DebugBackend> DebuggerCommand::backend() 
		{
			return debugger_->backend_;
		}

		size_t DebuggerCommand::skipSpaces(const std::string& line, size_t index)
		{
			while (index < line.length() && std::isspace(line[index]))
				index++;

			return index;
		}

		bool DebuggerCommand::parseUnsignedInteger(const std::string& token, uint64_t& value)
		{
			bool ret = false;
			size_t endpt;

			try
			{
				if (token.length() > 2 && token[0] == '0' && (token[1] == 'x' || token[1] == 'X'))
				{
					//
					// Hex
					//
					value = std::stoull(token.substr(2), &endpt, 16);
					if (endpt == token.length() - 2)
						ret = true;
				}
				else if (token.length() > 2 && token[0] == '0' && (token[1] == 'b' || token[1] == 'B'))
				{
					//
					// Binary
					//
					value = std::stoull(token.substr(2), &endpt, 2);
					if (endpt == token.length() - 2)
						ret = true;
				}
				else if (token.length() > 1 && token[0] == '0')
				{
					//
					// Octal
					//
					value = std::stoull(token.substr(1), &endpt, 8);
					if (endpt == token.length() - 1)
						ret = true;
				}
				else
				{
					//
					// Decimal
					//
					value = std::stoull(token, &endpt, 10);
					if (endpt == token.length())
						ret = true;
				}
			}
			catch (...)
			{
			}

			return ret;
		}

		std::vector<CmdArg> DebuggerCommand::toTokens(const std::string& line)
		{
			size_t index = 0, start;
			std::string token;
			std::vector<CmdArg> args;
			size_t endpt;

			while (index < line.length())
			{
				index = skipSpaces(line, index);
				if (index == line.length())
					break;

				start = index;
				while (index < line.length() && !std::isspace(line[index]))
					index++;

				token = line.substr(start, index - start);

				//
				// First try an unsigned value
				//
				{
					uint64_t value;

					if (parseUnsignedInteger(token, value))
					{
						args.push_back(CmdArg(value));
						continue;
					}
				}

				//
				// Next try a signed value
				//
				try
				{
					int64_t value = std::stoll(token, &endpt);
					if (endpt == token.length())
					{
						args.push_back(CmdArg(value));
						continue;
					}
				}
				catch (...)
				{
				}

				//
				// Next try a double value
				//
				try
				{
					double value = std::stod(token, &endpt);
					if (endpt == token.length())
					{
						args.push_back(CmdArg(value));
						continue;
					}
				}
				catch (...)
				{
				}

				//
				// Finally, none of these work, so the token is a string token
				//
				args.push_back(CmdArg(token));
			}

			return args;
		}

		std::string DebuggerCommand::toString(const std::vector<CmdArg>& args)
		{
			std::string ret;

			for (size_t i = 0; i < args.size(); i++)
			{
				const CmdArg& arg = args[i];

				if (i != 0)
					ret += ", ";
				ret += std::to_string(i) + ": ";
				if (std::holds_alternative<uint64_t>(arg))
				{
					ret += "0x" + bwg::misc::MiscUtilsHex::n2hexstr<uint64_t>(std::get<uint64_t>(arg));
					ret += " (uint64_t)";
				}
				else if (std::holds_alternative<int64_t>(arg))
				{
					ret += std::to_string(std::get<int64_t>(arg));
					ret += "( int64_t)";
				}
				else if (std::holds_alternative<double>(arg))
				{
					ret += std::to_string(std::get<int64_t>(arg));
					ret += "( double)";
				}
				else if (std::holds_alternative<std::string>(arg))
				{
					ret += std::get<std::string>(arg);
					ret += " (string)";
				}
				else
				{
					assert(false);
				}
			}

			return ret;
		}
	}
}
