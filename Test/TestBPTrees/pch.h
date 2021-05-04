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
#include "../../../CommonLib/filesystem/File.h"
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

#include "../../BPTrees/BPMap.h"
#include "../../BPTrees/BPIteratorMap.h"
#include "../../BPTrees/BPTreeMapLeafNode.h"
#include "../../BPTrees/BPTreeNodeMapHolder.h"


#include "../../BPTrees/BPSMap.h"
#include "../../BPTrees/BPTStringMapInnerNode.h"
#include "../../BPTrees/BPTStringMapLeafNode.h"
#include "../../BPTrees/Compressor/StringCompressor/ZLibStringEncoder.h"
#include "../TestDataGeneratorLib/RandomIntegerValueGenerator.h"
#include "../TestDataGeneratorLib/IntegerValueGenerator.h"
#include "../TestDataGeneratorLib/TestDataGenerator.h"

#pragma warning(disable:4146)

#include "../../BPTrees/BaseEncoder.h"
#include "../../BPTrees/BaseDIffEncoder.h"
#include "../../BPTrees/Compressor/EntropyEncoder/UnsignedNumLenEncoder.h"
#include "../../BPTrees/Compressor/EntropyEncoder/SignedNumLenEncoder.h"

#ifdef _WIN32
	#include "../../BPTrees/Compressor/ZlibCompressor/zstdencoder.h"
#endif


extern  CommonLib::CLogger Log;

static uint32_t nPageSize = 8192;

template<class _Ty>
struct comp
{
	bool LE(const _Ty& _Left, const _Ty& _Right) const
	{
		return (_Left < _Right);
	}
	bool EQ(const _Ty& _Left, const _Ty& _Right) const
	{
		return (_Left == _Right);
	}
	bool operator() (const _Ty& _Left, const _Ty& _Right) const
	{
		return (_Left < _Right);
	}
	/*bool MR(const _Ty& _Left, const _Ty& _Right){
	return (_Left > _Right);
	}*/
};
