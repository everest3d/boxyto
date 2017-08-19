/*****************************************************************************
The MIT License(MIT)

Copyright(c) 2016 Amr Essam

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

********************************************************************************/

#pragma once

// OS Detection
#ifdef _WIN32 
	//define something for Windows (32-bit and 64-bit, this part is common)
	#define OS_WINDOWS 
 
	#ifdef _WIN64 //(64-bit only)
		#define SYS_64BITS

	#else //(32-bit only)  
		#define SYS_32BITS

//
 
	#endif
#elif __APPLE__
	#include "TargetConditionals.h"
	#if TARGET_IPHONE_SIMULATOR
		#define OS_IOS
	// iOS Simulator 
	#elif TARGET_OS_IPHONE
		#define OS_IOS
	// iOS device
	#elif TARGET_OS_MAC
		#define OS_MACOSX
	// Other kinds of Mac OS
	#else
	#   error "Unknown Apple platform"
	#endif
#elif __linux__
	// linux
#elif __unix__ // all unices not caught above
	// Unix
#elif defined(_POSIX_VERSION)
	// POSIX
#else
	#   error "Unknown OS"
#endif

// Compiler detection
#if defined(__clang__)
/* Clang/LLVM. ---------------------------------------------- */
#define ALIGN_(x) __attribute__ ((align_value(x))) // Alignment

#elif defined(__ICC) || defined(__INTEL_COMPILER)
/* Intel ICC/ICPC. ------------------------------------------ */

#elif defined(__GNUC__) || defined(__GNUG__)
/* GNU GCC/G++. --------------------------------------------- */
#define ALIGN_(x) __attribute__ ((aligned(x))) // Alignment

#elif defined(__HP_cc) || defined(__HP_aCC)
/* Hewlett-Packard C/aC++. ---------------------------------- */

#elif defined(__IBMC__) || defined(__IBMCPP__)
/* IBM XL C/C++. -------------------------------------------- */

#elif defined(_MSC_VER)
/* Microsoft Visual Studio. --------------------------------- */
#define ALIGN_(x) __declspec(align(x)) // Alignment

#elif defined(__PGI)
/* Portland Group PGCC/PGCPP. ------------------------------- */

#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
/* Oracle Solaris Studio. ----------------------------------- */

#endif

// Defines

// Deprecated keyword for items no longer used
#define _DEPRECATED_

// ID used to define UINTs that refere to an dataset id
#define _ID_

// DEFINE THIS ONLY IF WE ARE USING ENGINE
// this flag is important to use debuging tools
//
// TODO: CONTROL THIS FLAG WHEN RELEASE
// TODO: we might put this flag in the project debug configuration
#define _ENGINE_USAGE_

// Specific modifier for engine functions
#define _engine_fun_ 

// function member output modifer
#ifndef _out_ref_ 
#define _out_ref_
#endif

#define _INLINE inline
#define _EVEREST Everest::  // Everst namespace shortcut
#define INDEX_NONE -1
#define DEFAULT_CONTAINER_CAPACITY 16
#define BAD_HANDLE -1 // No pointer handle

#define _CONST_VAR constexpr
#define _CONST_FUNC constexpr

// Primative Types
typedef unsigned char ubyte;
typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef long long int64;
typedef void* Pvoid;

template <typename U32BITS, typename U64BITS, uint8 size> // generic pointer int meta template
struct PtrIntType
{
	typedef U32BITS INTPRT;
};
template <typename U32BITS, typename U64BITS> // 32-bit meta template
struct PtrIntType<U32BITS, U64BITS, 4>
{
	typedef U32BITS Int;
};
template <typename U32BITS, typename U64BITS> // 64-bit  meta template
struct PtrIntType<U32BITS, U64BITS, 8>
{
	typedef U64BITS Int;
};

// size_t based on system pointer size
typedef PtrIntType<uint32, uint64, sizeof(void*)>::Int SIZE_T;

// singed size_t based on system pointer size
typedef PtrIntType<int32, int64, sizeof(void*)>::Int SSIZE_T;

// Define the pointer int type
typedef PtrIntType<uint32, uint64, sizeof(void*)>::Int UINTPTR;
typedef PtrIntType<int32, int64, sizeof(void*)>::Int SINTPTR;

// Pointer handle type to reference a Pointer<Type> from the pointer table
typedef int32 HPTR;

// Alignment values
enum { ALINGMENT_ZERO = 0, ALIGNMENT_DEFAULT = 8, ALIGNMENT_OPTIMAL = 16 };

// Macros

