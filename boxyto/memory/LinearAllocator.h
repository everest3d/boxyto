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
	class LinearAllocator
	{
	public:

		// typedefs
		typedef typename Pointer<T> pointer;

		// Options
		enum { OPTION_RESIZABLE = true };
		enum { OPTION_POOL = false };
		enum { OPTION_RECYCLE = false };

		// Allocator element size
		enum { ELEMENT_SIZE = sizeof(T) };

		// Rebind other instance
		template<class T2>
		struct Rebind
		{
			typedef LinearAllocator<T2> Other;
		};

		// Delete default constructor
		LinearAllocator() = delete;

		LinearAllocator(const LinearAllocator& other) :
			size(0),
			capacity(other.GetCapacity()),
			segmentManager(other.GetSegmentManager())
		{}

		// Create from segment manager
		LinearAllocator(std::shared_ptr<DynamicSegment> segment, uint32 initCapacity) :
			size(0),
			capacity(initCapacity)
		{
			segmentManager = segment;
		}

		// Destructor
		~LinearAllocator()
		{}

		// Initialize this allocator by allocating initCapacity
		void Init()
		{
			// LOG: Capacity is 0!
			// ...

			data = segmentManager->Alloc((capacity * ELEMENT_SIZE), alignof(T));
		}

		// Get Segment manager used by this allocator
		std::shared_ptr<DynamicSegment> GetSegmentManager() const
		{
			return segmentManager;
		}

		// Allocate count, reallocating all if required
		//
		// @param: count(1) - Count of elements to allocate
		// @return: Pointer to the allocator data
		pointer Allocate(uint32 count = 1)
		{
			// Check for realloc requirements
			CheckSize(count);

			return data;
		}

		// Deallocate count of element from total capacity
		//
		// @param: count(1) - Count of element to deallocate
		void Deallocate(uint32 count = 1)
		{
			size -= count;
		}

		// Clear all allocated element
		void Clear()
		{
			size = 0;
		}

		// Fills all elements of the container by an arbitary value
		//
		// @param: star - Index where to start the fill process, 0 if fill all elements
		// @param: count - How many elements to fill
		// @parma: value - the T value to fill in elements
		_INLINE void Fill(int32 start, uint32 count, const T& value)
		{
			ConstructRange(data.Get(), value, count);
		}

		//		I : Index					C : count				
		//		========I>>>>>>>>C===================
		//		_____________________________________
		//		|   |   | X | X |   |   |   |   |   |
		//		-------------------------------------
		//
		// Moves all elements start from index forward of the container by count,
		// use this function with caution.
		//
		// @param: index - the index to start shifting from
		// @param: count - count of elements to shift forward
		// @param: size - array size
		void ShiftForward(const uint32 index, const uint32 count, uint32 size)
		{
			uint32 shift = size - index - count;
			Memmove(Get() + index + count, Get() + index, ELEMENT_SIZE * shift);
		}

		//		C : count				I : Index
		//		========C<<<<<<<I===================
		//		_____________________________________
		//		|   |   | X | X |   |   |   |   |   |
		//		-------------------------------------
		//
		// Moves all elements start from index, to back by count
		// use this function with caution.
		//
		// @param: index - the index to start shifting from
		// @param: count - count of elements to shift back
		void ShiftBackward(const uint32 index, const uint32 count, uint32 size)
		{
			uint32 shift = size - index - 1;
			Memmove(Get() + index, Get() + (index + count), ELEMENT_SIZE * shift);
		}

		// Get a pointer to the first element of the container
		//
		// @return: pointer - to the first element in the container
		_INLINE pointer Get() const
		{
			return data;
		}

		// Return allocator capacity
		_INLINE uint32 GetCapacity() const
		{
			return capacity;
		}

	protected:

		// Check & allocate (if required) for addtional size for count of elements
		void CheckSize(uint32 count)
		{
			// Resize the allocator if required
			if ((size += count) > capacity)
				data = segmentManager->Realloc(data.GetHandle(), ELEMENT_SIZE * size, alignof(T));
		}

	private:

		// Segment manager instance
		std::shared_ptr<DynamicSegment> segmentManager;

		// The allocated handle for all elements
		pointer data;

		// Allocated elements count
		uint32 capacity;

		// Used elements count
		uint32 size;

	};
}