#pragma once
#ifdef WIN32
	#include "win/DynamicLibWin.h"
#else
	#include "linux/DynamicLibLin.h"
#endif

