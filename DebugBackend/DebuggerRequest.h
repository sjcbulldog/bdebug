#pragma once

#include <chrono>
#include <string>

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
				Success,
				Error
			};

		public:
			DebuggerRequest(BackendRequests req);
			virtual ~DebuggerRequest();

			RequestCompletionStatus status() const {
				return status_;
			}

			void setStatus(RequestCompletionStatus st) {
				status_ = st;
			}

			void waitFor() const;
			bool isComplete() const {
				return status_ == RequestCompletionStatus::Success || status_ == RequestCompletionStatus::Error;
			}

			BackendRequests type() {
				return req_;
			}

			std::string toString(RequestCompletionStatus st) {
				std::string ret = "Unknown";

				switch (st)
				{
				case RequestCompletionStatus::Queued:
					ret = "Queued";
					break;
				case RequestCompletionStatus::Running:
					ret = "Running";
					break;
				case RequestCompletionStatus::Success:
					ret = "Success";
					break;
				case RequestCompletionStatus::Error:
					ret = "Error";
					break;
				}

				return ret;
			}

			std::string toString(BackendRequests req) {
				std::string ret = "unknown";

				switch (req)
				{
				case BackendRequests::Reset:
					ret = "Reset";
					break;
				case BackendRequests::Stop:
					ret = "Stop";
					break;
				case BackendRequests::Run:
					ret = "Run";
					break;
				case BackendRequests::Breakpoint:
					ret = "Breakpoint";
					break;
				}

				return ret;
			}

			virtual std::string toString() {
				std::string ret;

				ret = "Packet, type=" + toString(req_) + ", status=" + toString(status_);
				return ret;
			}

		private:
			RequestCompletionStatus status_;
			BackendRequests req_;
		};
	}
}

