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

#include "DynamicSegment.h"
#include "../system.h"
#include "../Template/Common.h"
#include "MemoryOps.h"
#include "Pointer.h"

/* For std::shared_ptr */
#include <memory>

namespace Everest
{
	template <class T>
	class PoolNodeAllocator
	{
	public:

		// typedefs
		typedef typename Pair<UINTPTR, SIZE_T> Handle;
		typedef typename T ElementType;

		// Options
		enum { OPTION_RESIZABLE = true };
		enum { OPTION_POOL = true };
		enum { OPTION_RECYCLE = true };

		// Allocator element size
		enum { ELEMENT_SIZE = sizeof(T) };

		// Rebind other instance
		template<class T2>
		struct Rebind
		{
			typedef PoolNodeAllocator<T2> Other;
		};

		// Create from segment manager
		PoolNodeAllocator(std::shared_ptr<DynamicSegment> segment, uint32 capacity) :
			segmentManager(segment),
			data((UINTPTR)nullptr),
			recycledHeadOffset(0),
			nextFreeOffset(0),
			capacity(capacity)
		{}

		// Create from other same allocator
		PoolNodeAllocator(const PoolNodeAllocator& other) : 
			segmentManager(other.GetSegmentManager()),
			data((UINTPTR)nullptr),
			recycledHeadOffset(0),
			nextFreeOffset(0),
			capacity(other.GetCapacity())

		{}

		// Create from other allocator with other type
		template<class T2>
		PoolNodeAllocator(const PoolNodeAllocator<T2>& other) :
			segmentManager(other.GetSegmentManager()),
			data((UINTPTR)nullptr),
			recycledHeadOffset(0),
			nextFreeOffset(0),
			capacity(other.GetCapacity())
		{}

		// Destructor
		~PoolNodeAllocator()
		{
			// Deallocate allocator pool
			segmentManager->Dealloc(data);
		}

		// Delete = operator
		template<class T2>
		PoolNodeAllocator& operator= (const PoolNodeAllocator<T2>& other) = delete;

		// Initialize this allocator by allocating requrest capacity
		// This should called before using the allocator
		void Init()
		{
			// LOG CHECK(ELEMENT_SIZE >= sizeof(UINTPTR)
			//...

			// Allocate capacity
			data = segmentManager->Alloc(ELEMENT_SIZE * capacity, alignof(T));

			// Set next free element to the first element in the allocation
			//nextFreeOffset = (UINTPTR)SegmentManager::PointerOf(data);
		}

		// Get Segment manager used by this allocator
		std::shared_ptr<DynamicSegment> GetSegmentManager() const
		{
			return segmentManager;
		}

		// Return allocator capacity
		_INLINE uint32 GetCapacity() const
		{
			return capacity;
		}

		// Allocate a new Element Pointer by recycling or adding new
		//
		// @return - Pointer to the allocated Element
		Handle Allocate()
		{ 
			if (recycledHeadOffset) // Read element from recycle...
			{
				Handle result(data, recycledHeadOffset);
				recycledHeadOffset = MemRead<UINTPTR>(DynamicSegment::PointerOf(data), recycledHeadOffset);
				return result;
			}
			else // Allocate new element...
			{
				// TODO: Expanding allocator if required
				
				Handle result(data, nextFreeOffset);
				nextFreeOffset += ELEMENT_SIZE;
				return result;
			}
		}

		// Allocate and construct, a new node
		//
		// @param: args... - The constructor arguments
		// @return: Allocator pointer to object
		template<class ...Args>
		Handle Allocate(Args && ...args)
		{
			UINTPTR handle = Allocate();
			new(Parse(handle)) T(_EVEREST Forward<Args>(args)...);
			return handle;
		}

		// Deallocate an element by recycling it
		//
		//@param: Pointer to element to deallocate & recycle
		void Deallocate(Handle handle)
		{
			UINTPTR next = (recycledHeadOffset ? recycledHeadOffset : (UINTPTR)nullptr);
			MemWrite(next, DynamicSegment::PointerOf(data), handle._second);
			recycledHeadOffset = handle._second;
		}

		// Parse a handle and return it's pointer
		// The handle must be previously allocated using this allocator type
		//
		// @param: handle - The handle to parse
		// @return: Parsed pointer of type T
		static T* Parse(const Handle& handle)
		{
			return reinterpret_cast<T*>(((UINTPTR)DynamicSegment::PointerOf(handle._first)) + handle._second);
			//return (T*)(((UINTPTR)SegmentManager::PointerOf(handle._first)) + handle._second);
			//return  Offset(reinterpret_cast<T*>(SegmentManager::PointerOf(handle._first)), handle._second);
		}

		// Null handle
		static const Handle null_handle() { return Handle(0,0); }

		struct Parser
		{
			typedef typename Handle Handle;

			// Parse a handle and return it's pointer
			// The handle must be previously allocated using this allocator type
			//
			// @param: handle - The handle to parse
			// @return: Parsed pointer of type T
			template <class Type>
			static Type* Parse(const Handle& handle)
			{
				return (Type*)(((UINTPTR)DynamicSegment::PointerOf(handle._first)) + handle._second);
			}

			static bool IsNull(Handle handle)
			{
				return handle == null_handle();
			}
		};

	private:

		// Segment manager instance
		std::shared_ptr<DynamicSegment> segmentManager;

		// The allocated handle for all elements
		UINTPTR data;

		// Recycled linked-list head
		SIZE_T recycledHeadOffset;

		// Next free element at the end of the allocation
		// We use this ONLY if no recycled elements yet
		SIZE_T nextFreeOffset;

		// Allocator capacity
		uint32 capacity;

	};
}