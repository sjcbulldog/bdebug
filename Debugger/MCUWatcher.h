#pragma once

#include "DebugBackend.h"
#include <thread>

namespace bwg
{
	namespace debug
	{
		class MCUWatcher
		{
		public:
			MCUWatcher(std::shared_ptr<bwg::backend::DebugBackend> backend);
			virtual ~MCUWatcher();

			void stop() {
				running_ = false;
				thread_.join();
			}

		private:
			void run();

		private:
			std::thread thread_;
			std::shared_ptr<bwg::backend::DebugBackend> backend_;
			bool running_;
		};
	}
}
