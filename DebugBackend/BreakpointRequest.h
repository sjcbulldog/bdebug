#pragma once

#include "BreakpointType.h"
#include "DebuggerRequest.h"

namespace bwg
{
	namespace backend
	{
		class BreakpointRequest : public DebuggerRequest
		{
		public:
			BreakpointRequest(BreakpointType type, uint64_t addr, uint64_t size) : DebuggerRequest(DebuggerRequest::BackendRequests::Breakpoint) {
				type_ = type;
				addr_ = addr;
				size_ = size;
			}

			virtual ~BreakpointRequest() {
			}

			BreakpointType type() const {
				return type_;
			}

			uint32_t addr32() const {
				return static_cast<uint32_t>(addr_);
			}

			uint64_t addr() const {
				return addr_;
			}

			uint32_t size32() const {
				return static_cast<uint32_t>(size_);
			}

			uint64_t size() const {
				return size_;
			}

		private:
			BreakpointType type_;
			uint64_t addr_;
			uint64_t size_;
		};
	}
}
