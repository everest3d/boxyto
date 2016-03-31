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
#include "DynamicSegment.h"
#include "../containers/Pair.h"

namespace Everest
{
	template <class T>
	class Pointer
	{
	public:

		typedef typename Pointer type;

		Pointer() :
			handle(0)
		{}

		// Constructor that allows pointer creation with handle
		//
		// @param: handle - Pointer handle.
		Pointer(UINTPTR handle) :
			handle(handle)
		{}

		Pointer(const Pointer& _other) :
			handle(_other.handle)
		{}

		template<class T2>
		Pointer(const Pointer<T2>& _other) :
			handle(_other.handle)
		{}

		// Operator for equality check with other pointer,
		// two pointers are equal if thier IDs & _Pointers are equal
		_INLINE const bool operator== (const Pointer& _other) const
		{
			return (handle == _other.handle);
		}

		// Operator check if this pointer is not equal with other pointer,
		// two pointers are not equal if thier IDs & _Pointers are not equal
		_INLINE bool operator!= (const Pointer& _other)
		{
			return  (handle != _other.handle);
		}

		// Operator equal that copy other pointer
		// This copy updated automatically when another copy is updated
		_INLINE Pointer& operator=(const Pointer& _other)
		{
			if (this != &_other)
			{
				handle = _other.handle;
			}
			return *this;
		}

		// Return the pointer object
		_INLINE T* Get() const
		{
			return (T*)DynamicSegment::PointerOf(handle);
		}

		// pointer access operator
		_INLINE T* operator->() const
		{
			return Get();
		}

		UINTPTR GetHandle() const
		{
			return handle;
		}

	private:

		UINTPTR handle;
	};

	template <class T>
	class OffsetPointer
	{
	public:

		OffsetPointer() :
			handle(0, 0)
		{}

		// Constructor that allows pointer creation with handle
		//
		// @param: allocHandle - Allocator segment handle.
		// @param: elementOffset - This element offset
		OffsetPointer(UINTPTR allocHandle, SIZE_T elementOffset) :
			handle(Pair<UINTPTR, SIZE_T>(allocHandle, elementOffset))
		{}

		OffsetPointer(const OffsetPointer& _other) :
			handle(_other.handle)
		{}

		template<class T2>
		OffsetPointer(const OffsetPointer<T2>& _other) :
			handle(_other.handle)
		{}

		// Operator for equality check with other pointer,
		// two pointers are equal if thier IDs & _Pointers are equal
		_INLINE const bool operator== (const OffsetPointer& _other) const
		{
			return (Get() == _other.Get());
		}

		// Operator check if this pointer is not equal with other pointer,
		// two pointers are not equal if thier IDs & _Pointers are not equal
		_INLINE bool operator!= (const OffsetPointer& _other)
		{
			return  (Get() != _other.Get());
		}

		// Operator equal that copy other pointer
		// This copy updated automatically when another copy is updated
		_INLINE OffsetPointer& operator=(const OffsetPointer& _other)
		{
			if (this != &_other)
			{
				handle = _other.handle;
				//_pointer = _other._pointer;
			}
			return *this;
		}

		// Return the pointer object
		_INLINE T* Get() const
		{
			return Offset(reinterpret_cast<T*>(DynamicSegment::PointerOf(handle._first)), handle._second);
		}

		// pointer access operator
		_INLINE  T* operator->() const
		{
			return Get();
		}

		Pair<UINTPTR, SIZE_T> GetHandle() const
		{
			return handle;
		}

	private:

		// First -> Allocator head
		// Second -> Element pointer offset
		Pair<UINTPTR, SIZE_T> handle;

	}; // Pointer

	template <class Parser, class ElementType>
	class NodePointer
	{
	public:
		typedef typename Parser::Handle Handle;

		NodePointer() = default;

		NodePointer(Handle handle) :
			handle(handle)
		{}

		NodePointer(const NodePointer& other) :
			handle(other.handle)
		{}

		NodePointer& operator= (const NodePointer& other)
		{
			if (this != &other)
				handle = other.handle;

			return *this;
		}

		// Operator for equality check with other pointer,
		// two pointers are equal if thier IDs & _Pointers are equal
		_INLINE bool operator== (const NodePointer& _other) 
		{
			return (handle == _other.handle);
		}

		// Operator check if this pointer is not equal with other pointer,
		// two pointers are not equal if thier IDs & _Pointers are not equal
		_INLINE bool operator!= (const NodePointer& _other)
		{
			return  (handle != _other.handle);
		}

		// Return the pointer object
		_INLINE ElementType* Get() const
		{
			return Parser::Parse<ElementType>(handle);
		}

		// pointer access operator
		_INLINE  ElementType* operator->() const
		{
			return Get();
		}

		// Return reference to this pointer's handle
		Handle GetHandle() const
		{
			return handle;
		}

		_INLINE bool IsNull() const
		{
			return Parser::IsNull(handle);
		}

	private:
		Handle handle;
	};
}