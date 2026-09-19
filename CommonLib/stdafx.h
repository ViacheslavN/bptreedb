// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#ifndef __linux__
	#include "targetver.h"
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "CommonLib.h"
#include "../CommonLib/alloc/alloc.h"
#include "exception/exc_base.h"

#ifdef _WIN32
	#include "exception/WinExc.h"
	#include "exception/WinNtExc.h"
	#include "exception/ComExc.h"
#endif



// reference additional headers your program requires here
