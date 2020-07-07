#include "DebuggerRequest.h"
#include <cassert>
#include <thread>

namespace bwg
{
	namespace backend
	{
		DebuggerRequest::DebuggerRequest(BackendRequests req)
		{
			status_ = RequestCompletionStatus::Queued;
			req_ = req;
		}

		DebuggerRequest::~DebuggerRequest()
		{
		}

		void DebuggerRequest::waitFor() const 
		{
			while (status_ != RequestCompletionStatus::Error && status_ != RequestCompletionStatus::Success)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}
}
