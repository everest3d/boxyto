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

#include "StaticSegment.h"
#include "MemoryOps.h"

#include <memory>

namespace Everest
{
#pragma warning( push )
#pragma warning( disable : 4003)
	template <class T>
	class StdAllocator
	{
	public:

		// typedefs
		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;

		// Convert an StdAllocator<T> to StdAllocator<T2>
		template<typename T2>
		struct rebind {
			typedef StdAllocator<T2> other;
		};

		inline explicit StdAllocator() {}
		inline explicit StdAllocator(std::shared_ptr<StaticSegment> segment) :
			segmentManager(segment)
		{}
		inline ~StdAllocator() {}
		inline explicit StdAllocator(StdAllocator const& other) :
			segmentManager(other.GetSegmentManager())
		{}
		template<typename T2>
		inline explicit StdAllocator(StdAllocator<T2> const& other) :
			segmentManager(other.GetSegmentManager())
		{}

		// Address
		inline pointer address(reference ref) { return &ref; }
		inline const_pointer address(const_reference ref) { return &ref; }

		// Memory allocation
		inline pointer allocate(size_type cnt,
			const void* = 0) {
			//return reinterpret_cast<pointer>(
			//	SegmentManager::PointerOf(
			//		segmentManager->Alloc((cnt * sizeof(T)), alignof(T))));
			return (pointer)segmentManager->Alloc((cnt * sizeof(T)), alignof(T));
		}
		inline void deallocate(pointer p, size_type) {
			/* Nothing to do here */
		}

		// Size
		inline size_type max_size() const {
			return ((size_t)(-1) / sizeof(T));
		}

		// Construction/destruction
		inline void construct(pointer p, const T& t) { _EVEREST Construct(p, t); }
		inline void destroy(pointer p) { _EVEREST Destroy(p, 1); }

		inline bool operator==(StdAllocator const&) { return true; }
		inline bool operator!=(StdAllocator const& a) { return !operator==(a); }

		// Get Segment manager used by this allocator
		std::shared_ptr<StaticSegment> GetSegmentManager() const
		{
			return segmentManager;
		}

	private:

		std::shared_ptr<StaticSegment> segmentManager;

	};
#pragma warning( pop ) 
}