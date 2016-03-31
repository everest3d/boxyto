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

/* For std::shared_ptr */
#include <memory>

namespace Everest
{
	template <class T>
	class SimpleNodeAllocator
	{
	public:

		// typedefs

		typedef typename UINTPTR Handle;
		typedef typename T ElementType;

		// Options
		enum { OPTION_RESIZABLE = false};
		enum { OPTION_POOL = false };
		enum { OPTION_RECYCLE = false };

		// Allocator element size
		enum { ELEMENT_SIZE = sizeof(T) };

		// Rebind other instance
		template<class T2>
		struct Rebind
		{
			typedef SimpleNodeAllocator<T2> Other;
		};

		// Create from segment manager
		SimpleNodeAllocator(std::shared_ptr<DynamicSegment> segment) :
			segmentManager(segment)
		{}

		// Create from other same allocator
		SimpleNodeAllocator(const SimpleNodeAllocator& other) :
			segmentManager(other.GetSegmentManager())
		{}

		// Create from other allocator with other type
		template<class T2>
		SimpleNodeAllocator(const SimpleNodeAllocator<T2>& other) :
			segmentManager(other.GetSegmentManager())
		{}

		template<class T2>
		SimpleNodeAllocator& operator= (const SimpleNodeAllocator<T2>& other)
		{
			if (this != &other)
			{
				this->segmentManager = other.segmentManager;
			}

			return *this;
		}

		// Destructor
		~SimpleNodeAllocator()
		{
			/* Nothing to do here */
		}

		// Initialize this allocator by the included segment
		void Init()
		{
			/* Nothing to do here */ 
		}

		// Get Segment manager used by this allocator
		std::shared_ptr<DynamicSegment> GetSegmentManager() const
		{
			return segmentManager;
		}

		// Allocate a new node
		//
		// @return: Allocator handle to node
		Handle Allocate()
		{
			return segmentManager->Alloc(ELEMENT_SIZE, alignof(T));
		}

		// Allocate and construct, a new node
		//
		// @param: args... - The constructor arguments
		// @return: Allocator pointer to object
		template<class ...Args>
		Handle Allocate(Args && ...args)
		{
			UINTPTR handle = segmentManager->Alloc(ELEMENT_SIZE, alignof(T));
			new(Parse(handle)) T(_EVEREST Forward<Args>(args)...);

			return handle;
		}

		// Deallocate an object from the allocator and release memory to the segment manager
		//
		// @param: handle - Handle to node
		void Deallocate(Handle handle)
		{
			segmentManager->Dealloc(handle);
		}

		// Parse a handle and return it's pointer
		// The handle must be previously allocated using this allocator type
		//
		// @param: handle - The handle to parse
		// @return: Parsed pointer of type T
		static T* Parse(const Handle& handle)
		{
			return reinterpret_cast<T*>(DynamicSegment::PointerOf(handle));
		}

		// Null handle
		static const Handle null_handle() { return (Handle)nullptr; }

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
				return reinterpret_cast<Type*>(DynamicSegment::PointerOf(handle));
			}

			static bool IsNull(Handle handle)
			{
				return handle == null_handle();
			}
		};


	private:

		// Segment manager instance
		std::shared_ptr<DynamicSegment> segmentManager;

	};
}