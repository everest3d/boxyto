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
#include "../Template/Common.h"
#include "Array.h"

namespace Everest
{
	template < class T, class Allocator >
	class CircularArray
	{
	public:

		typedef T ElementType;

		// Delete equal constructor
		CircularArray() = delete;

		// Construct with given capacity
		CircularArray(uint32 capacity, Allocator alloc) :
			_capacity(capacity),
			_size(0),
			_front(0),
			_rear(0),
			_elements(capacity, alloc)
		{
		}

		// Initialize by coping from other container,
		// container should has [] operator & Size() & support iteration
		//
		// @param: _container - the other container to fill from
		template < class Container >
		CircularArray(const Container& _container) :
			_capacity(_container.Size()),
			_size(0),
			_front(0),
			_rear(0),
			_elements(_capacity, alloc)
		{
			auto it = _container.CreateConstIterator();
			for (; it != _container.cEnd(); ++it)
				Enqueue(*it);
		}

		~CircularArray()
		{}

		// Insert an element at the front of the array
		//
		// @param: element - reference to element to insert
		// @return: true if inserted
		bool Enqueue(const ElementType& element)
		{
			if (_size == _capacity)
				return false;

			_elements[_rear] = element;
			_rear = (_rear + 1) % _capacity;
			_size++;

			return true;
		}

		// Insert an element at the front of the array, Move temp
		//
		// @param: element - reference to element to insert
		// @return: true if inserted
		_INLINE bool Enqueue(const ElementType&& element)
		{
			return Enqueue(_EVEREST Move(element));
		}

		// Remove an element from the array
		//
		// @return: true if removed
		bool Dequeue()
		{
			if (Empty())
				return false;

			_elements.Destroy(_front, 1);
			_front = (_front + 1) % _capacity;
			_size--;

			return true;
		}

		// Return true if array is empty
		_INLINE bool Empty()
		{
			return _size == 0;
		}

		// Return a reference to the internal array
		_INLINE const auto& GetArray()
		{
			return _elements;
		}

	private:

		Array<ElementType, Allocator> _elements;

		uint32 _size, _capacity, _front, _rear;
	};
}