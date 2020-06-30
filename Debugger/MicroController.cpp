#include "MicroController.h"
#include "Logger.h"
#include "ElfReader.h"
#include "Debugger.h"

using namespace bwg::backend;
using namespace bwg::logfile;
using namespace bwg::elf;

namespace bwg
{
	namespace debug
	{
		MicroController::MicroController(Debugger *debug, const std::string& mcutag, const MCUDesc &desc) : desc_(desc)
		{
			debug_ = debug;
			mcutag_ = mcutag;
		}

		MicroController::~MicroController()
		{
		}

		bool MicroController::loadElfFile(const std::filesystem::path& path)
		{
			ElfReader reader(elffile_, path, debug_->logger());
			return reader.read();
		}
	}
}
