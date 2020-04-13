#pragma once

#include <iostream>
#include "../../../CommonLib/CommonLib.h"
#ifdef _WIN32
	#include "../../../CommonLib/CommonLib_lib.h"
#endif
#include "../../../CommonLib/exception/exc_base.h"
#include "../../../CommonLib/log/logger.h"
#include "../../../CommonLib/log/ConsoleLogWriter.h"
#include "../../../CommonLib/log/LogInfo.h"
#include "../../../CommonLib/perf/PerfCount.h"
#include "../../utils/CacheLRU_2Q.h"
#include "../../storage/FileStorage.h"
#include "../../utils/ReadStreamPage.h"
#include "../../utils/WriteStreamPage.h"
#ifdef _WIN32
	#include "../../bptreedb_lib.h"
#endif
#include "../../BPTrees/BPSet.h"
#include "../../BPTrees/BPTreeInnerNode.h"
#include "../../BPTrees/BPTreeLeafNode.h"
#include "../../BPTrees/BaseNodeCompressor.h"
#include "../../BPTrees/BaseLeafNodeSetCompressor.h"
#include "../../BPTrees/Compressor/ZlibCompressor/ZLibEncoder.h"

#ifdef _WIN32
	#include "../../BPTrees/Compressor/ZlibCompressor/zstdencoder.h"
#endif


extern  CommonLib::CLogger Log;
