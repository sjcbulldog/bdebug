#include "DebuggerRequest.h"

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
	}
}
