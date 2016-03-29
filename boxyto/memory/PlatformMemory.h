#pragma once

#include "../system.h"

#ifdef OS_WINDOWS
	#include <Windows.h>

#elif OS_MACOSX
	#include <stdlib.h>

#elif OS_IOS
	#include <stdlib.h>

#else
	#include <stdlib.h>
	#include <malloc.h>

#endif