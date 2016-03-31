/*****************************************************************************
The MIT License(MIT)

Copyright(c) 2016 Amr Esam

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

*********************************************************************************/
#pragma once

#include "../system.h"
#include <cstring>

namespace Everest
{
	// System memcpy  
	static void* Memcopy(void* dst, void* src, const SIZE_T size)
	{
		return memcpy(dst, src, size);
	}
	 
	// System memmove
	static void* Memmove(void* dst, void* src, const SIZE_T size)
	{
		return memmove(dst, src, size);
	}

	// System memcmp
	static int Memcmp(const void* p1, const void* p2, SIZE_T size)
	{
		return memcmp(p1, p2, size);
	}

	// Get an offset memory location before or after given pointer
	//
	// @param: prt - pointer to offset
	// @param: value - the offset value
	// @return: The offseted pointer
	template <typename T>
	_INLINE T* Offset(const T* ptr, SSIZE_T value)
	{
		return (T*)(((UINTPTR)ptr) + value);
	}

	// Write a value of type T to memory stream at position
	//
	// @param: val - The value to write
	// @param: stream - Pointer to stream to write into
	// @param: position - The location to start write value
	template <typename T>
	_INLINE void MemWrite(const T& val, void* stream, SSIZE_T position)
	{
		memcpy(Offset(stream, position), &val, sizeof(T));
	}

	// Read value from memory stream, and return a reference to it
	//
	// @param: stream - Pointer to stream to read from
	// @param: position - The location to start read value
	template <typename T>
	_INLINE T& MemRead(const void* stream, SSIZE_T position)
	{
		return *reinterpret_cast<T*>(Offset(stream, position));
	}

	// Align, in place a givent pointer of some type to the next valid alignment
	// thanks to : http://www.songho.ca/misc/alignment/dataalign.html 
	//
	// @param: ptr - pointer to the type to align
	// @param: alignment - The desired value to align with
	// @return: pointer to the aligned address of type T
	template <typename T>
	_INLINE T* Align(T * ptr, int32 alignment)
	{
		return (T*)(((UINTPTR)ptr + (alignment - 1)) & ~(alignment - 1));
	}

	// Align, in place a givent pointer of some type to the next valid alignment
	// this version return misalignment space
	//
	// @param: ptr - pointer to the type to align
	// @param: alignment - The desired value to align with
	// @param: space - ref to return misalignment space
	// @return: pointer to the aligned address of type T
	template <typename T>
	_INLINE T* Align(T * ptr, int32 alignment, _out_ref_ uint8& space)
	{
		space = (uint8)(alignment - ((UINTPTR)ptr & (alignment - 1)));
		return (T*)((UINTPTR)ptr + space);
	}

	// Align, and Write alignment, in place a givent pointer of some type to the next valid alignment
	// this version return and write at ptr[-1] misalignment space
	//
	// @param: ptr - pointer to the type to align
	// @param: alignment - The desired value to align with
	// @return: pointer to the aligned address of type T
	template <typename T>
	_INLINE T* AlignW(T * ptr, int32 alignment)
	{
		uint8 space = 0;
		ptr = Align(ptr, alignment, space);
		MemWrite(space, ptr, -1);
		return ptr;
	}

	// Unalign previously aligned pointer with AlignW
	// 
	// @param: ptr - Pointer to unalign
	// @return - UNaligned pointer
	template <typename T>
	_INLINE T* UnalignW(const T* ptr)
	{
		uint8 space = MemRead<uint8>(ptr, -1);
		return (T*)((UINTPTR)ptr - space);
	}

	// Calls an object constructor and pass it's arguments
	//
	// @param: element - pointer to the element to construct
	// @param: args - element's constructor arguments
	template < class T, class ... Args >
	_INLINE void Construct(T* element, Args&&... args)
	{
		new (element) T(_EVEREST Forward<Args>(args)...);
	}

	// Construct contiguous elements from the same type, in memory, usful for copy constructors
	//
	// @param: dest - The start pointer to construct
	// @param: value - The element's value to construct with
	// @param: count - Element count
	template < class T, class ... Args >
	_INLINE void ConstructRange(T* dest, const T& value, uint32 count)
	{
		while (count--)
			new (dest++) T(value);
	}

	// Linear constructing items in dest from items in source, for a range of elements from a range of elements
	// Elements should be the same type
	//
	// @param: source - Source to construct from
	// @param: dest - The start pointer to construct
	// @param: value - The element's value to construct with
	// @param: count - Element count
	template < class T>
	_INLINE void ConstructRangeFromRange(T* source, T* dest, uint32 count, uint32 start = 0)
	{
		while (count--)
		{
			new (dest + (start++)) ElementType(*source);
			++source;
		}
	}

	// Calls an element destructor for range of elements
	//
	// @param: startElement - pointer to the first element to destruct
	// @param: count(1) - how many elements to destruct
	template< class T >
	_INLINE void Destroy(T* startElement, uint32 count = 1)
	{
		while (count--)
		{
			startElement->~T();
			++startElement;
		}
	}
}
