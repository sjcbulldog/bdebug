#pragma once

namespace bwg
{
	namespace backend
	{
		enum class BreakpointType
		{
			Software,
			Hardware,
			Read,
			Write,
			Access
		};
	}
}