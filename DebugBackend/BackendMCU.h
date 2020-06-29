#pragma once

#include <string>

namespace bwg
{
	namespace backend
	{
		class BackendMCU
		{
		public:
			enum BreakpointType
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

			void setStop(bool reset, bool main) {
				stop_at_reset_ = reset;
				stop_at_main_ = main;
			}

			bool stopAtReset() const {
				return stop_at_reset_;
			}

			bool stopAtMain() const {
				return stop_at_main_;
			}

			virtual bool readVectorTable() = 0;
			virtual bool run(bool wait) = 0 ;
			virtual bool restart() = 0;
			virtual bool setBreakpoint(BreakpointType type, uint32_t addr, uint32_t size) = 0 ;

		private:
			std::string tag_;
			bool stop_at_reset_;
			bool stop_at_main_;
		};
	}
}
