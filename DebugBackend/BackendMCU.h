#pragma once

#include <string>

namespace bwg
{
	namespace backend
	{
		class BackendMCU
		{
		public:
			enum class BreakpointType
			{
				Software,
				Hardware,
				Read,
				Write,
				Access
			};

		public:
			BackendMCU(const std::string& tag) {
				tag_ = tag;
			}

			virtual ~BackendMCU() {

			}

			const std::string tag() const {
				return tag_;
			}

			virtual bool readVectorTable() = 0;
			virtual bool run() = 0 ;
			virtual bool stop() = 0;
			virtual bool restart() = 0;
			virtual bool setBreakpoint(BreakpointType type, uint32_t addr, uint32_t size) = 0 ;

		private:
			std::string tag_;
		};
	}
}
