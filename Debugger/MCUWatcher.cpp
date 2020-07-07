#include "MCUWatcher.h"
#include "Message.h"
#include <cassert>

using namespace bwg::backend;
using namespace bwg::logfile;

namespace bwg
{
	namespace debug
	{
		MCUWatcher::MCUWatcher(std::shared_ptr<DebugBackend> backend)
		{
			backend_ = backend;
			running_ = true;
			thread_ = std::thread(&MCUWatcher::run, this);
		}

		MCUWatcher::~MCUWatcher()
		{
		}

		void MCUWatcher::run()
		{
			int current = 0;
			int previous = 0;

			auto mcus = backend_->mcus();
			for (auto mcu : mcus)
			{
				assert(mcu->state() != BackendMCU::MCUState::Running);
			}

			while (running_)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));

				mcus = backend_->mcus();
				for (auto mcu : mcus)
				{
					if (mcu->state() == BackendMCU::MCUState::Running)
						current++;
				}

				if (current != previous)
				{
					Message msg(Message::Type::Debug, "watcher");
					msg << "running thread count changed " << previous << " -> " << current;
					backend_->logger() << msg;
				}

				if (current < previous)
				{
					//
					// Someone stopped, stop everyone else
					//
					for (auto mcu : mcus)
					{
						if (mcu->state() == BackendMCU::MCUState::Running)
							mcu->request(DebuggerRequest::BackendRequests::Stop);
					}

					//
					// Give the MCUs a change to stop
					//
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
				else if (current > previous)
				{
					//
					// We are starting to run, its ok
					//
				}

				previous = current;
				current = 0;
			}
		}
	}
}
