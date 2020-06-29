#include "Message.h"

namespace bwg
{
	namespace logfile
	{
		Message::Message(Message::Type type, const std::string& module)
		{
			type_ = type;
			module_ = module;
		}

		Message::~Message()
		{
		}

		std::string Message::messageString() const
		{
			std::string txt;

			switch (type_)
			{
			case Message::Type::Debug:
				txt = "debug:";
				break;
			case Message::Type::Error:
				txt = "error:";
				break;
			case Message::Type::Warning:
				txt = "warning:";
				break;
			case Message::Type::Info:
				txt = "info:";
				break;
			case Message::Type::Fatal:
				txt = "falta:";
				break;
			}

			txt += " " + module_ + ": ";
			txt += text_;

			return txt;
		}
	}
}
