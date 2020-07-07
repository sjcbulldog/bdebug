#pragma once

#include "BreakpointType.h"
#include "DebuggerRequest.h"
#include <string>
#include <list>
#include <memory>
#include <mutex>

namespace bwg
{
	namespace backend
	{
		class DebugBackend;

		class BackendMCU
		{
			friend class DebugBackend;

		public:
			enum class MCUState
			{
				Initializing,
				FailedConnect,
				FailedInitialization,
				FailedGeneral,
				WaitingForReset,
				Reset,
				Stopped,
				Running,
			};

		public:
			BackendMCU(DebugBackend &be, const std::string& mcutag, bool master) : backend_(be) {
				mcutag_ = mcutag;
				state_ = MCUState::Initializing;
				master_ = master;
			}

			virtual ~BackendMCU() {
			}

			bool isMaster() const {
				return master_;
			}

			MCUState state() const {
				return state_;
			}

			const std::string mcutag() const {
				return mcutag_;
			}

			//
			// This is how requsts cross the boundary between threads
			//
			virtual std::shared_ptr<DebuggerRequest> request(std::shared_ptr<DebuggerRequest> req) {
				{
					std::lock_guard<std::mutex> guard(lock_);
					requests_.push_back(req);
				}

				signal_.notify_one();
				return req;
			}

			virtual std::shared_ptr<DebuggerRequest> request(DebuggerRequest::BackendRequests req) {
				auto reqptr = std::make_shared<DebuggerRequest>(req);
				return request(reqptr);
			}

		protected:
			DebugBackend &backend() {
				return backend_;
			}

			void setState(MCUState st) {
				state_ = st;
			}

			size_t requestCount() {
				return requests_.size();
			}

			std::shared_ptr<DebuggerRequest> next() {
				std::lock_guard<std::mutex> guard(lock_);

				if (requests_.size() == 0)
					return nullptr;

				auto req = requests_.front();
				requests_.pop_front();
				return req;
			}

		private:
			std::mutex lock_;
			std::string mcutag_;
			MCUState state_;
			bool master_;

			std::condition_variable signal_;
			std::list<std::shared_ptr<DebuggerRequest>> requests_;

			DebugBackend& backend_;
		};
	}
}
