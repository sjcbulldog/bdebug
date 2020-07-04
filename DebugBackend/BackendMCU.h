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
		class BackendMCU
		{
		public:
			enum class MCUState
			{
				Initializing,
				FailedConnect,
				FailedInitialization,
				FailedGeneral,
				Running,
			};

		public:
			BackendMCU(const std::string& mcutag, bool master) {
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
				std::lock_guard<std::mutex> guard(lock_);
				requests_.push_back(req);

				return req;
			}

			virtual std::shared_ptr<DebuggerRequest> request(DebuggerRequest::BackendRequests req) {
				auto reqptr = std::make_shared<DebuggerRequest>(req);
				return request(reqptr);
			}

			virtual std::shared_ptr<DebuggerRequest> response() {
				std::lock_guard<std::mutex> guard(lock_);

				if (responses_.size() == 0)
					return nullptr;

				auto front = responses_.front();
				responses_.pop_front();

				return front;
			}

		protected:

			//
			// These all happen in the per MCU thread
			//
			virtual bool reset() = 0;
			virtual bool provideSymbols() = 0;

			virtual bool run() = 0;
			virtual bool stop() = 0;
			virtual bool setBreakpoint(BreakpointType type, uint32_t addr, uint32_t size) = 0 ;


		protected:
			void setState(MCUState st) {
				state_ = st;
			}

		private:
			std::mutex lock_;
			std::string mcutag_;
			MCUState state_;
			bool master_;

			std::list<std::shared_ptr<DebuggerRequest>> requests_;
			std::list<std::shared_ptr<DebuggerRequest>> responses_;
		};
	}
}
