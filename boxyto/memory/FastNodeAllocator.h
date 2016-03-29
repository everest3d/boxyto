#pragma once

#include "StaticSegment.h"
#include "../system.h"
#include "../Template/Common.h"
#include "MemoryOps.h"
#include "Pointer.h"

/* For std::shared_ptr */
#include <memory>

namespace Everest
{
	template <class T>
	class FastNodeAllocator
	{
	public:

		// typedefs
		typedef typename UINTPTR Handle;
		typedef typename T ElementType;

		// Options
		enum { OPTION_RESIZABLE = false };
		enum { OPTION_POOL = true };
		enum { OPTION_RECYCLE = false };

		// Allocator element size
		enum { ELEMENT_SIZE = sizeof(T) };

		// Rebind other instance
		template<class T2>
		struct Rebind
		{
			typedef FastNodeAllocator<T2> Other;
		};

		// Construct from segment manager
		FastNodeAllocator(std::shared_ptr<StaticSegment> segment) :
			segmentManager(segment)
		{}

		FastNodeAllocator(FastNodeAllocator& other) :
			segmentManager(other.GetSegmentManager())

		{}

		// Create from other allocator with other type
		template<class T2>
		FastNodeAllocator(const FastNodeAllocator<T2>& other) :
			segmentManager(other.GetSegmentManager())
		{}

		template <class T2> 
		FastNodeAllocator& operator= (FastNodeAllocator&) = delete;

		void Init()
		{
			/* Nothing to do here */
		}

		// Get Segment manager used by this allocator
		std::shared_ptr<StaticSegment> GetSegmentManager() const
		{
			return segmentManager;
		}

		// Allocate a new node
		//
		// @return: Allocator handle to node
		Handle Allocate()
		{
			return (Handle)segmentManager->Alloc(ELEMENT_SIZE, alignof(T));
		}

		void Deallocate(Handle handle)
		{
			/* Nothing to do here */
		}

		// Parse a handle and return it's pointer
		// The handle must be previously allocated using this allocator type
		//
		// @param: handle - The handle to parse
		// @return: Parsed pointer of type T
		static T* Parse(const Handle& handle)
		{
			return (T*)handle;
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
				return (Type*)handle;
			}

			static bool IsNull(Handle handle)
			{
				return handle == null_handle();
			}
		};


	private:

		// Segment manager instance
		std::shared_ptr<StaticSegment> segmentManager;


	};
}