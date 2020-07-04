#pragma once

#include <chrono>

namespace bwg
{
	namespace backend
	{
		class DebuggerRequest
		{
		public:
			enum class BackendRequests
			{
				Reset,
				Stop,
				Run,
				Breakpoint,
			};

			enum class RequestCompletionStatus
			{
				Queued,
				Running,
			};

		public:
			DebuggerRequest(BackendRequests req);
			virtual ~DebuggerRequest();

			RequestCompletionStatus status() const {
				return status_;
			}

			bool waitFor(std::chrono::milliseconds timeout) const;
			bool waitFor() const;
			bool isComplete() const {
				return status_ != RequestCompletionStatus::Running && status_ != RequestCompletionStatus::Queued;
			}

		private:
			RequestCompletionStatus status_;
			BackendRequests req_;
		};
	}
}

