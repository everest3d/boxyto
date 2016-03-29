#pragma once

#include "../system.h"
#include "../Template/Common.h"
#include "../memory/MemoryOps.h"

namespace Everest
{
	template < class T >
	class linearIterator
	{
		typedef linearIterator<T> ConstIterator;

	public:
		typedef linearIterator _MyT;
		typedef T ElementType;
		typedef T& reference;
		typedef T* pointer;
		//typedef unsigned int difference_type;

		linearIterator(pointer ptr, int32 index) : _ptr(ptr), _index(index)
		{ }

		_MyT operator++()
		{
			_MyT i = *this;
			_ptr++; _index++;
			return i;
		}

		_MyT operator++(int junk)
		{
			_ptr++; _index++; return *this;
		}

		_MyT operator--()
		{
			_MyT i = *this;
			_ptr--; _index--;
			return i;
		}

		_MyT operator--(int junk)
		{
			_ptr--; _index-- return *this;
		}

		reference operator*()
		{
			return *_ptr;
		}

		pointer operator->()
		{
			return _ptr;
		}

		bool operator==(const _MyT& rhs)
		{
			return _ptr == rhs._ptr;
		}

		bool operator!=(const _MyT& rhs)
		{
			return _ptr != rhs._ptr;
		}

		_MyT operator-(const uint32 offset)
		{
			_index -= offset; return _ptr -= offset;
		}

		_MyT operator+(const uint32 offset)
		{
			_index += offset; return _ptr += offset;
		}

		_MyT& operator-=(uint32 offset)
		{
			_index -= offset; return _ptr -= -offset;
		}

		_MyT& operator+=(uint32 offset)
		{
			_index += offset; return _ptr += offset;
		}

		bool operator>(_MyT other)
		{
			return (void*)_ptr > (void*)other._ptr;
		}

		bool operator<(_MyT other)
		{
			return (void*)_ptr < (void*)other._ptr;
		}

		_INLINE int32 GetIndex()
		{
			return _index;
		}

		_INLINE const int32 GetIndex() const
		{
			return _index;
		}

	private:
		pointer _ptr;
		int32 _index;
	};



	template < class T >
	class linearReverseIterator
	{
	public:
		typedef linearReverseIterator<T> ConstIterator;

	public:
		typedef linearReverseIterator<T> _MyT;
		typedef T ElementType;
		typedef T& reference;
		typedef T* pointer;
		//typedef unsigned int difference_type;

		linearReverseIterator(pointer ptr, int32 index) : _ptr(ptr), _index(index)
		{ }

		_MyT operator++()
		{
			_MyT i = *this;
			_ptr--; _index--;
			return i;
		}

		_MyT operator++(int junk)
		{
			_ptr--; _index--; return *this;
		}

		_MyT operator--()
		{
			_MyT i = *this;
			_ptr++; _index++;
			return i;
		}

		_MyT operator--(int junk)
		{
			_ptr++; _index++ return *this;
		}

		reference operator*()
		{
			return *_ptr;
		}

		pointer operator->()
		{
			return _ptr;
		}

		bool operator==(const _MyT& rhs)
		{
			return _ptr == rhs._ptr;
		}

		bool operator!=(const _MyT& rhs)
		{
			return _ptr != rhs._ptr && _index == _index;
		}

		_MyT operator-(const uint32 offset)
		{
			_index += offset; return _ptr -= offset;
		}

		_MyT operator+(const uint32 offset)
		{
			_index -= offset; return _ptr += offset;
		}

		_MyT& operator-=(uint32 offset)
		{
			_index += offset; return _ptr -= -offset;
		}

		_MyT& operator+=(uint32 offset)
		{
			_index -= offset; return _ptr += offset;
		}

		bool operator>(_MyT other)
		{
			return (void*)_ptr > (void*)other._ptr;
		}

		bool operator<(_MyT other)
		{
			return (void*)_ptr < (void*)other._ptr;
		}

		_INLINE int32 GetIndex()
		{
			return _index;
		}

		_INLINE const int32 GetIndex() const
		{
			return _index;
		}

	private:
		pointer _ptr;
		int32 _index;
	};

   /*
	* Array Class, can be work as static or dynamic array,
	* Use this container to memic the std::array for static
	* and std::vector for dynamc arrays.
	*
	* Array class has many features that make access the container easy,
	* It also has some of most common java or .net containers functions.
	* Also array has alignment feature, and integrated with Boxitoo memory manager.
	*
	* Array class can also used as stack, with methods like pop, push, top.
	*
	* Array class is a cross-platform container, work with 32bits & 64bits systems
	* with no problems
	*
	* @author: Amr Esam
	*/
	template < class T, class AllocatorType >
	class Array
	{
		template<class Other_T, class OtherAllocator>
		friend class TArray;

public:
		// Typedefs

		typedef Array<T, AllocatorType> _MyT;
		typedef T ElementType;
		typedef ElementType& reference;
		typedef ElementType&& rvalue;
		typedef const ElementType& const_reference;

		typedef typename AllocatorType Allocator;

		typedef linearIterator < ElementType > iterator;
		typedef linearIterator <const ElementType > const_iterator;

		typedef linearReverseIterator < ElementType > reverse_iterator;
		typedef linearReverseIterator < const ElementType > const_reverse_iterator;

		//typedef typename DefaultLinearAllocator<ElementType>::template Type Allocator;

	
		// Defult Constructor
		Array(Allocator alloc) :
			_size(0),
			_capacity(0),
			_allocator(alloc)
		{
			_allocator.Init();
		}

		// Copy from other array constructor
		//
		// @param: other - Array to copy from
		Array(Array& other) :
			_capacity(other.Capacity()),
			_size(0),
			_allocator(other.GetAllocator())
		{
			_allocator.Init();
			_allocator.Allocate(_capacity);

			// Copy from other to this array
			Append(other);
		}

		// Create the array by reversing uninitialized elements
		//
		// @param: other - Array to copy from
		// @param: capacity - the initial capacity to add at the end of the array
		Array(const uint32 capacity, Allocator alloc) :
			_capacity(capacity),
			_size(0),
			_allocator(alloc)
		{
			_allocator.Init();
			_allocator.Allocate(_capacity);
		}

		// Destruct all items and free allocation
		~Array()
		{
			_EVEREST Destroy(Data(), _size);
			_allocator.Clear();
		}

		// Assign a default value to all array element
		// 
		// @param: value - reference value of ElementType
		void Assign(reference value)
		{
			_allocator.Fill(0, _size, value);
		}

		// Construct in place an element at the end of the array
		//
		// @param: args - The element to create and construct
		// @returns: index of the added element
		template<class... Args>
		int32 Emplace(Args&&... args)
		{
			const int32 index = AddUninitialized();
			new(Data() + index)
				ElementType(_EVEREST Forward <Args>(args)...);
			return index;
		}

		// Copying add reference element 
		// 
		// @param: element - rererence element to copy
		// return: index of the added element
		_INLINE int32 Add(const_reference element)
		{
			return Emplace(element);
		} 

		//TODO: AddUnique

		// Moving add rvalue element 
		// 
		// @param: element - rvalue element to move
		// return: index of the added element
		_INLINE int32 Add(rvalue element)
		{
			return Emplace(_EVEREST Move(element));
		}

		// Appending a reference array of the same type
		// 
		// @param: other - rererence element to copy
		// return: index of the added elements
		int32 Append(const _MyT& other)
		{
			//check if other and this are the same
			if (this == &other || other.Size() == 0)
				return INDEX_NONE;

			// Add uninitialize elements
			int32 index = AddUninitialized(other.Size());

			// Construct from other array
			ConstructRangeFromRange(other.Data(), Data(), other.Size(), index);

			return index;
		}

		// Appending a reference array of the same type
		// 
		// @param: other - rererence element to copy
		// return: index of the added element
		int32 Append(_MyT&& right)
		{
			//check if other and this are the same
			if (this == &other || other.Size() == 0)
				return INDEX_NONE;

			// Add uninitialize elements
			int32 index = AddUninitialized(other.Size());

			// Construct from other array
			ConstructRangeFromRange(other.Data(), Data(), other.Size(), index);

			//destruct right array
			right.Release();

			// Index of the first element inserted
			return index;
		}

		// Insert a copy of an element at index
		// The whole array may be reallocated if needed
		// 
		// @param: element - The element to copy and insert
		// @param: where - Index to insert in
		_INLINE int32 Insert(const_reference element, int32 where)
		{
			if (where < 0 || where > _size)
				return INDEX_NONE;

			const int32 index = InsertUninitialized(where);

			new(Data() + index) ElementType(element);

			return index;
		}

		// Insert a move of an element at index
		// The whole array may be reallocated if needed
		// 
		// @param: element - The element to move and insert
		// @param: where - Index to insert in
		int32 Insert(rvalue element, int32 where)
		{
			if (where < 0 || where > _size)
				return INDEX_NONE;

			const int32 index = InsertUninitialized(where);

			new(_allocator.Get() + index) ElementType(_EVEREST Move(element));

			return index;
		}

		// Insert other array in this array start from index
		// 
		// @param: other - the array to insert
		// @param: where - index to start insertion
		// @return: inserted at index
		int32 Insert(const _MyT& other, int32 where)
		{
			if (where < 0 || where > _size)
				return INDEX_NONE;

			// Insert other size
			where = InsertUninitialized(where, other.Size());

			// Copy from other to this array
			_allocator.ConstructRange(other.Data(), other.Size(), where);
			///Memory::Memcopy(Data() + where, other.Data(), other.Size());

			return where;
		}

		// Insert from other array in this array by copying 
		// start from index up to count
		// 
		// @param: other - the array to insert
		// @param: where - index to start insertion
		// @param: count - count of element to insert from other
		// @return: inserted at index
		int32 Insert(_MyT& other, int32 where, const uint32 count)
		{
			if (where < 0 || where > _size)
				return INDEX_NONE;

			// Insert other size
			where = InsertUninitialized(where, count);

			// Copy from other to this array
			ConstructRangeFromRange(other.Data(), Data(), count, where);
			///Memory::Memcopy(Data() + where, other.Data(), other.Size());

			return where;
		}

		// Insert from other array by moving elements to this array and remove them
		// from other array.
		//
		// @param: other - the array to insert
		// @param: otherWhere - index to start insert from where
		// @param: where - index to start insertion
		// @param: count - count of element to insert from other
		// @param: ShrinkOther(false) - if true, reset the allocator and free all memory
		// @return: inserted at index
		int32 InsertMove(_MyT& other, int32 otherWhere, int32 where, const uint32 count, bool ShrinkOther = false)
		{
			//if (where < 0 || where > _size 
			//	|| otherWhere < 0 || otherWhere > other.Size())
			//	return INDEX_NONE;

			// Insert other size
			where = InsertUninitialized(where, count);

			Memory::Memmove(Data() + where, other.Data() + otherWhere, count * GetElementSize());

			other.Remove(otherWhere, count, ShrinkOther);

			return where;
		}

		// Insert an element start at where, and repeat it for count elements
		// Elements addes has the same value, but different copies
		//
		// @param: element - the element to repeat
		// @param: where - index to start repeat
		// @param: count - how many elements to repeat
		int32 Repeat(const_reference element, int32 where, uint32 count)
		{
			if (where < 0 || where > _size)
				return INDEX_NONE;

			where = InsertUninitialized(where, count);

			_EVEREST ConstructRange(Data() + where, element, count);
			//while (count--)
			//{
			//	new(Data() + where)ElementType(element);
			//	where++;
			//}

			return where;
		}

		// Resetting the array by destroying all items and release from allocator
		void Release()
		{
			Clear();

			_allocator.Clear();

			_size = _capacity = 0;
		}

		// Remove element or more from this array
		// this method also destuct removed element(s) 
		// 
		// @param: index - start index to remove from
		// @param: count(1) - count of elements to remove
		// @param: shrinkToFit(false) - if true, reset the allocator and free all memory
		void Remove(const int32 index, const int32 count = 1, bool shrinkToFit = false)
		{
			// Check if index & count exists
			if (index >= _size)
				return;

			// Destroy item
			_EVEREST Destroy(Data() + index, count);

			// Shift elements back, if needed - optmization
			if ((index + count) == _size)
				; // Last elements removed, no need to shift elements
			else
				_allocator.ShiftBackward(index, count, Size());

			// Handle shrinking
			if (shrinkToFit)
			{
				_allocator.Deallocate(count);
				_capacity -= count;
			}

			// Update size
			_size -= count;
		}

		// Remove element or more from this array using start iterator
		// this method also destuct removed element(s) 
		// 
		// @param: start - start iterator to remove from
		// @param: count(1) - count of elements to remove
		// @param: shrinkToFit(false) - if true, reset the allocator and free all memory
		_INLINE void Remove(iterator start, uint32 count = 1, bool shrinkToFit = false)
		{
			Remove(start.GetIndex(), count, shrinkToFit);
		}

		// Call default constructor for a range of elements 
		//
		// @param: index - start index to construct from
		// @param: count(1) - count of elements to construct
		/*void ConstructEmpty(int32 start, uint32 count = 1)
		{
			start = InsertUninitialized(start, count);

			while (count--)
			{
				new(Data() + start)ElementType();
				start++;
			}
			 
		}*/

		// Destory (not remove) a range of elements 
		//
		// @param: index - start index to destroy from
		// @param: count(1) - count of elements to destroy
		//_INLINE void Destroy(const int32 index, const uint32 count = 1)
		//{
		//	_EVEREST Destroy(Data() + index, count);
		//}

		// Swap two elements at given indecies
		// Move temp swap
		// 
		// @param: first - first element to swap
		// @param: second - element to swap with first
		void Swap(uint32 first, uint32 second)
		{
			// Validate first & second are indcies
			if (first == second || first > _size || second > _size)
				return;

			ElementType temp = _EVEREST Move(Data()[first]);
			Data()[first] = _EVEREST Move(Data()[second]);
			Data()[second] = _EVEREST Move(temp);
		}

		// Replace by moving count elements from other to this array,
		// values will be replaced
		//
		// @param: other - Array to replace from
		// @param: otherWhere - Other start index
		// @param: where - index to start insert in this array
		// @param: count - how many elements to replace
		// @param: shinkOther(false) - if true, reset the allocator and free all memory
		void Move(_MyT& other, int32 otherWhere, int32 where, int32 count, bool ShrinkOther = false)
		{
			// Ensure capacity
			if ((count + where) <= _size)
				;
			else
				AddUninitialized(_size - (count + where));

			_EVEREST Memmove(Data() + where, other.Data() + otherWhere, count * GetElementSize());

			other.Remove(otherWhere, count, ShrinkOther);
		}

		// Clear all elements possibly destruct all elements, but leave capacity as it is
		void Clear()
		{
			_EVEREST Destroy(Data(), _size);
			_size = 0;
		}

		// Shrinks array to fit it's size
		void ShrinkToFit()
		{
			if (_capacity == _size)
				return;

			_allocator.Deallocate(_capacity - _size);
			_capacity = _size;
		}

		// Reserve count of array capacity
		// This function will just ensure that array has empty count elements
		//
		// @param: count(1) - count of elements to reserve
		void Reserve(uint32 count = 1)
		{
			_allocator.Allocate(count);

			_capacity += count;
		}

		// Ensure that array contains empty elements of count
		// and reverse if not
		void Ensure(uint32 count)
		{
			uint32 empty = _capacity - _size;
			if (count > empty)
				Reserve(count - empty);
		}

		// Check if array is empty
		_INLINE bool Empty()
		{
			return (_size == 0);
		}

		// Checks if the array is full,
		// means size = capacity
		_INLINE bool Full()
		{
			return (_size == _capacity);
		}

		_INLINE bool ContainIndex(const int32 index)
		{
			return (index >= 0 && index < _size);
		}

		// Check if this array contains an element
		// NOTE: This method assume that element overload operator ==
		// @param: element - The element to check
		// @return: true if found
		bool Contains(const_reference element)
		{
			const_iterator itStart = CreateConstIterator();
			for (; itStart != cEnd(); ++itStart)
				if (element == *itStart)
					return true;

			return false;
		}

		// Finds an element searching forward the array, return it's index if found
		// 
		// @param: element - the element to find
		// @param: start - index to start forward search from
		// @return: index of the element of found, -1 otherwise
		int32 Find(const_reference element, int32 start = 0)
		{
			const_iterator itStart = CreateConstIterator(start);
			for (; itStart != cEnd(); ++itStart)
			{
				if (element == *itStart)
					return start;

				start++;
			}

			return INDEX_NONE;
		}

		// Finds an element searching backward the array, return it's index if found
		// 
		// @param: element - the element to find
		// @return: index of the element of found, -1 otherwise
		int32 FindLast(const_reference element)
		{
			const_reverse_iterator itStart = CreateConstReverseIterator();
			uint32 index = _size - 1;
			for (; itStart != crEnd(); ++itStart)
			{
				if (element == *itStart)
					return index;

				index--;
			}

			return INDEX_NONE;
		}

		//TODO: Find by Predicate

		// Copies a range of elements from other array to this array  
		//
		// @param: start - index to start copy
		// @param: count - count of elements to copy
		// @return: new array with the copies elements
		template<typename T, typename _Allocator>
		void CopyFrom(const Array<T, _Allocator>& source, uint32 start, uint32 count)
		{
			if (start > source._size || count > source._size - start)
				return;

			while (count--)
				Add(source[start++]);
		}

		// Return index of a given element
		// If element is not exsits, returns -1
		//
		// @param: element - The element to get index for
		// @return: the index of element if found, -1 otherwise (INDEX_NONE)
		int32 IndexOf(const_reference element)
		{
			int32 index = 0;
			const_iterator itStart = CreateConstIterator();
			const_iterator itEnd = CreateConstIterator(_size);

			for (; itStart != itEnd; ++itStart)
			{
				if (*itStart == element)
					return index;

				index++;
			}

			return INDEX_NONE;
		}

		// Return an iterator start from some index
		//
		// @param: start - index to start element
		// @return: Iterator to the element at index start
		iterator CreateIterator(uint32 start = 0)
		{
			return iterator(Data() + start, start);
		}

		// Return a const iterator start from some index
		//
		// @param: start - index to start element
		// @return: Const Iterator to the element at index start
		const_iterator CreateConstIterator(uint32 start = 0) const
		{
			return const_iterator(Data() + start, start);
		}

		// Return a reverse iterator start from some index
		//
		// @return: reverse Iterator to the element at index start
		reverse_iterator CreateReverseIterator()
		{
			return rBegin();
		}

		// Return a reverse iterator start from some index
		//
		// @param: start - index to start element
		// @return: reverse Iterator to the element at index start
		reverse_iterator CreateReverseIterator(int32 start)
		{
			int32 index = _size - start;
			return reverse_iterator(Data() + index, index);
		}

		// Return a const reverse iterator start from some index
		//
		// @param: start - index to start element
		// @return: Const reverse Iterator to the element at index start
		const_reverse_iterator CreateConstReverseIterator() const
		{
			return crBegin();
		}

		// Return a const reverse iterator start from some index
		//
		// @param: start - index to start element
		// @return: Const reverse Iterator to the element at index start
		const_reverse_iterator CreateConstReverseIterator(int32 start) const
		{
			uint32 index = _size - start;
			return const_reverse_iterator(Data() + index, index);
		}

		// Returns an iterator to the begin of the array
		iterator Begin()
		{
			return iterator(Data(), 0);
		}

		// Returns a const iterator to the begin of the array
		const_iterator cBegin() const
		{
			return const_iterator(Data(), 0);
		}

		// Returns an iterator to the end of the array
		iterator End()
		{
			return iterator(Data() + _size, _size);
		}

		// Returns a const iterator to the end of the array
		const_iterator cEnd() const
		{
			return const_iterator(Data() + _size, _size);
		}

		// Returns a reverse iterator, start from the end of the array
		reverse_iterator rBegin()
		{
			return reverse_iterator(Data() + (_size - 1), (_size - 1));
		}

		// Returns a const reverse iterator, start from the end of the array
		const_reverse_iterator crBegin() const
		{
			return const_reverse_iterator(Data() + (_size - 1), (_size - 1));
		}

		// Returns a reverse iterator, start from the begining of the array
		reverse_iterator rEnd()
		{
			return reverse_iterator(Data() - 1, -1);
		}

		// Returns a const reverse iterator, start from the begining of the array
		const_reverse_iterator crEnd() const
		{
			return const_reverse_iterator(Data() - 1, -1);
		}

		// Returns reference to the first element from Array
		// Stack method
		//
		// @return: reference element
		reference Top()
		{
			return Data()[_size - 1];
		}

		// Returns const reference to the first element from Array
		// Stack method
		//
		// @return: const reference element
		const_reference Top() const
		{
			return (const ElementType&)Data()[_size - 1];
		}

		// Add element to the stack
		// Stack method
		//
		// @param: element - const reference to element to add
		void Push(const_reference element)
		{
			Add(element);
		}

		// Add element to the stack, by move temp
		// Stack method
		//
		// @param: element - rvalue to element to add
		void Push(rvalue element)
		{
			Add(_EVEREST Move(element));
		}

		// Remove and return the last in element of the stack
		// Stack method
		//
		// @param: shrinkToFit - allows container to shrink to size
		ElementType Pop(bool shrinkToFit = true)
		{
			int32 index = _size - 1;
			ElementType temp = _EVEREST Move(Data()[index]);
			Remove(index, shrinkToFit);
			return temp;
		}

		// Returns reference to the front element from List
		// Queue method
		//
		// @return: reference element
		_INLINE ElementType& Front()
		{
			return Top();
		}

		// Returns const reference to the front element from array
		// Queue method
		//
		// @return: const reference element
		_INLINE const ElementType& Front() const
		{
			return Top();
		}

		// Insert an element at the front of the array
		// Queue method
		//
		// @param: element - reference to element to insert
		void Enqueue(const ElementType& element)
		{
			Insert(element, 0);
		}

		// Insert an element at the front of the array, Move temp
		// Queue method
		//
		// @param: element - reference to element to insert
		void Enqueue(const ElementType&& element)
		{
			Insert(_EVEREST Move(element), 0);
		}

		// Remove an element from the front of the list
		// Queue method
		void Dequeue()
		{
			Remove(_size - 1);
		}

		// Returns readonly element at index
		_INLINE const_reference operator[](const int32 index) const
		{
			if (_size <= index)
				return *End();

			return Data()[index];
		}

		// Returns a reference of element at index
		_INLINE reference operator[](int32 index)
		{
			
			return *(Data()+ index);
		}

		// Appends an other array at the end of this array
		//
		// @param: other - array to append
		// @return: reference to this array
		_INLINE _MyT& operator+=(_MyT& other)
		{
			Append(other);
			return *this;
		}

		// Appends an other array at the end of this array( Move version )
		//
		// @param: right - array to append by move
		// @return: reference to this array
		_INLINE _MyT& operator+=(_MyT&& right)
		{
			Append(_EVEREST Move(right));
			return *this;
		}

		// Construct this array by other array
		//
		// @param: other - array to construct from
		// @return: reference to this array
		_INLINE _MyT& operator=(const _MyT& other)
		{
			if (this != &other)
			{
				Release(true);
				Append(other);
			}
			return *this;
		}

		// Construct this array by other array ( Move versoipm )
		//
		// @param: other - array to construct from
		// @return: reference to this array
		_INLINE _MyT& operator=(_MyT&& right)
		{
			if (this != &right)
			{
				Release(true);
				Append(_EVEREST Move(right));
			}
			return *this;
		}

		// Check if this array is equal to other array
		// equality determined by comparing size and memory location
		//
		// @param: other - array to compare with
		// @return: bool - true if equal, otherwise false
		_INLINE bool operator==(const _MyT& other)
		{
			return (_size == other.Size())
				&& (Data() == other.Data())
		}

		// Return a pointer to the first element of the array
		_INLINE ElementType* Data()
		{
			return _allocator.Get().Get();
		}

		// Return a const pointer to the first element of the array
		_INLINE const ElementType* Data() const
		{
			return (const ElementType*)_allocator.Get().Get();
		}

		// Return array's element data type size in bytes
		_INLINE SIZE_T GetElementSize() const
		{
			return sizeof(ElementType);
		}

		// Returns array total memory size in bytes,
		// this is includes capacity allocation
		_INLINE SIZE_T GetDataSize() const
		{
			return sizeof(sizeof(ElementType) * _capacity);
		}

		// Return a reference to this array allocator instance
		_INLINE Allocator& GetAllocator() const
		{
			return _allocator;
		}

		// Returns the array capacity
		_INLINE uint32 Capacity() const { return _capacity; }

		// Returns the actual size of the array
		// (elements that ONLY constructed)
		_INLINE uint32 Size() const { return _size; }

	protected:

		// Add uninitialized element(s) at the end of the array
		// elements addes can not be used until initialized
		//
		// @param: count - How many elements to add
		// @return: index of the first element added
		int32 AddUninitialized(const uint32 count = 1)
		{
			// store index for the first item to add
			int32 index = _size;

			Ensure(count);

			_size += count; // new size

			return index;
		}

		// Insert uninitialized element(s) at some index
		// 
		// @param: where - index to insert at
		// @param: count - count of elements to insert
		int32 InsertUninitialized(int32 where, uint32 count = 1)
		{
			// Add space to elements 
			AddUninitialized(count);

			// Shift elements back, if needed
			if ((where + count) == _size)
				;
			else
				// move the rest of the elements to insert new ones
				//Memmove(Data() + where + count, Get() + index, _elementByteSize * shift);
				_allocator.ShiftForward(where, count, _size);
				

			return where;
		}

		// Check if index in array range and validate it if not in range
		//
		// @param: index - byref index to validate
		_INLINE void ValidateIndex(int32& index)
		{
			if (_size - index > 1)
				index = _size;
			else if (index < 0)
				index = 0;
		}

	private:

		// Array actual size
		// This is the count of constructed elements
		int32 _size;

		// Array all elements size
		// This is the size of all elements,
		// constructed and created only items
		int32 _capacity;

		// Allocator instance
		Allocator _allocator;

	}; // Array
}