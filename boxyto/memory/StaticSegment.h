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
#include "OSMemory.h"
#include "../Template/Common.h"

namespace Everest
{
	class StaticSegment
	{

	private:

		// Pointer to the allocator memory memory ready for allocation
		void* memory;

		// Segment size in bytes
		SIZE_T segmentSize;

		// Size allocated in bytes
		SIZE_T allocationSize;

	public:

		// Default constructor that allocate segment to one huge system page as default size
		StaticSegment() :
			memory(nullptr),
			segmentSize(OSMemory::GetPageSize()),
			allocationSize(0)
		{
			// Reserve memory from OS to this segment
			memory = OSMemory::RequestSegment(segmentSize);
		}

		// Construct segment with count of segments
		// size = count * system-page-size
		//
		// @param: segCount - Count of segments to allocate, one segment = default page size
		StaticSegment(uint32 segCount) :
			memory(nullptr),
			segmentSize(segCount * OSMemory::GetPageSize()),
			allocationSize(0)
		{
			// Reserve memory from OS to this segment
			memory = OSMemory::RequestSegment(segmentSize);
		}

		// Delete equality constructor
		StaticSegment(const StaticSegment&) = delete;
		StaticSegment& operator= (const StaticSegment&) = delete;

		// StaticSegment destructor
		~StaticSegment()
		{
			Release();
		}

		// Release this segment to the os memory
		// The allocations in this segment is no longer valid
		void Release()
		{
			OSMemory::ReleaseSegment(memory);
		}

		// Allocate memory using this segment's private memory
		// The allocation is aligned by alignment
		// Please note that there is no Dealloc in this segment
		//
		// @param: size - The size in bytes to allocate
		// @param: alignment(16) - The alignment of the allocation
		// @return: Pointer to the allocated memory
		void* Alloc(SIZE_T size, uint32 alignment = ALIGNMENT_OPTIMAL)
		{
			// LOG: check alignment is a power of 2 ?

			// Update size with requested + alignment
			SIZE_T newSize = size + alignment;

			// Check for out of memory
			if ((allocationSize + newSize) > segmentSize)
			{
				// LOG: OUT OF MEMORY
				return nullptr;
			}

			// Update allocationSize
			allocationSize += newSize;

			// Write allocation size at the begining of memory
			//MemWrite(size, memory, 0);

			// Store memory to return
			void* result = memory;

			// Update memory pointer to the new ready-to-allocate pointer
			memory = Offset(memory, newSize);

			// Return aligned pointer
			return AlignW(result, alignment);
		}

		// Return an allocation size allocated by a StaticSegment
		//
		// @param: ptr - pointer to get size of
		// @return: Allocation size in bytes
		//static SIZE_T SizeOf(void* ptr)
		//{
		//	if (ptr != nullptr)
		//		return MemRead<SIZE_T>(ptr, -(SSIZE_T)(sizeof(SIZE_T)));
		//
		//	// LOG: ERR Get size of null pointer: StaticSegment::SizeOf(ptr)
		//	return 0;
		//}

	};
}