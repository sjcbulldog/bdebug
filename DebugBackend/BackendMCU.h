#pragma once

#include "BreakpointType.h"
#include <string>

namespace bwg
{
	namespace backend
	{
		class BackendMCU
		{
		public:


		public:
			BackendMCU(const std::string& mcutag) {
				mcutag_ = mcutag;
			}

			virtual ~BackendMCU() {

			}

			const std::string mcutag() const {
				return mcutag_;
			}

			virtual bool readVectorTable() = 0;
			virtual bool setThreadParams() = 0;
			virtual bool provideSymbols() = 0;
			virtual bool run() = 0 ;
			virtual bool stop() = 0;
			virtual bool reset() = 0;
			virtual bool setBreakpoint(BreakpointType type, uint32_t addr, uint32_t size) = 0 ;
			virtual bool waitForStop() = 0 ;

		private:
			std::string mcutag_;
		};
	}
}
