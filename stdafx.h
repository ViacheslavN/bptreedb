// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN32
	#include "targetver.h"
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#include "../CommonLib/CommonLib.h"
#include "../CommonLib/exception/exc_base.h"
#include "../CommonLib/alloc/alloc.h"
#include "../CommonLib/perf/PerfCount.h"




// TODO: reference additional headers your program requires here
