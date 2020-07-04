#include "MCUWatcher.h"

using namespace bwg::backend;

namespace bwg
{
	namespace debugger
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
			while (running_)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}
	}
}
