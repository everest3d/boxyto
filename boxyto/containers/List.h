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
#include "../memory/MemoryOps.h"
//#include "Tree.h"
#include "Pair.h"

/* For std::shared_ptr */
#include <memory>

namespace Everest
{
	template < class T, class HandleType >
	class DoubleLinkedListNode
	{
	public: 

		// Typedefs
		typedef DoubleLinkedListNode<T, HandleType> Node;
		typedef T ElementType;
		typedef HandleType Handle;

		// Construct node from element, next, and previous nodes
		DoubleLinkedListNode(ElementType element, Handle next, Handle previous) :
			_element(element),
			_next(next),
			_previous(previous)
		{}

		// Construct node from next, previous nodes
		DoubleLinkedListNode(Handle next, Handle previous) :
			_next(next),
			_previous(previous)
		{}

		// Construct node from element, null for next and previous
		/*DoubleLinkedListNode(ElementType element) :
			_element(element),
			_next(nullptr),
			_previous(nullptr)
		{}*/

		// Delete default constructor
		DoubleLinkedListNode() = default;

		// Retrun a pointer to the next node
		_INLINE Handle GetNext()
		{
			return _next;
		}

		// Retrun a const pointer to the next node
		_INLINE const Handle GetNext() const
		{
			return _next;
		}

		// Return a pointer to the previous node
		_INLINE Handle GetPrev()
		{
			return _previous;
		}

		// Return a const pointer to the previous node
		_INLINE const Handle GetPrev() const
		{
			return _previous;
		}

		// Set the next node pointer
		_INLINE void SetNext(const Handle& _other)
		{
			_next = _other;
		}

		// Set the previous node pointer
		_INLINE void SetPrev(const Handle& _other)
		{
			_previous = _other;
		}

		// Return a reference to the node's element
		_INLINE ElementType& GetElement()
		{
			return _element;
		}

		// Return a const reference to the node's element
		_INLINE const ElementType& GetElement() const
		{
			return _element;
		}

		// In place element constuction
		template<class... Args>
		void ConstuctElement(Args&&... args)
		{
			new(&_element)
				ElementType(_EVEREST Forward <Args>(args)...);
		}

		void DestructElement()
		{
			(&_element)->~ElementType();
		}

		// Operator checks if this node is equal to other node
		// return true of equal
		//TODO:
		bool operator==(const Node& _other)
		{
			return true;
		}

	private:

		Handle _next, _previous;
		ElementType _element;

	}; //DoubleLinkedListNode


	template < class T, class AllocatorType >
	class DoubleLinkedListIterator
	{
	public:

		// Typedefs
		typedef DoubleLinkedListIterator _MyT;
		typedef T Element;

		typedef typename AllocatorType::Handle Handle;

		DoubleLinkedListIterator(Handle current) :
			_Current(current)
		{}

		_MyT operator++(int junk)
		{
			_MyT _Tmp = *this;
			++*this;
			return _Tmp;
		}

		_MyT operator++()
		{
			_Current = AllocatorType::Parse(_Current)->GetNext();
			return *this;
		}

		_MyT operator--(int junk)
		{
			_Current = AllocatorType::Parse(_Current)->GetPrev();
			return *this;
		}

		_MyT operator--()
		{
			_Current = AllocatorType::Parse(_Current)->GetPrev();
			return *this;
		}

		Element& operator*()
		{
			return AllocatorType::Parse(_Current)->GetElement();
		}

		const Element& operator*() const
		{
			return AllocatorType::Parse(_Current)->GetElement();
		}

		Element* operator->()
		{
			return &(AllocatorType::Parse(_Current)->GetElement());
		}

		bool operator!=(const _MyT& other)
		{
			return _Current != other._Current;
		}

		_INLINE Handle GetNode() const
		{
			return _Current;
		}

	private:
		Handle _Current;
	}; // LinkedListIterator

	template < class T, class AllocatorType >
	class DoubleLinkedListRevereseIterator
	{
	public:

		// Typedefs
		typedef DoubleLinkedListRevereseIterator _MyT;
		typedef T Element;
		
		typedef typename AllocatorType::Handle Handle;

		DoubleLinkedListRevereseIterator(Handle current) :
			_Current(current)
		{}

		_MyT operator++(int junk)
		{
			_Current = AllocatorType::Parse(_Current)->GetPrev();
			return *this;
		}

		_MyT operator++()
		{
			_Current = AllocatorType::Parse(_Current)->GetPrev();
			return *this;
		}

		_MyT operator--(int junk)
		{
			_Current = AllocatorType::Parse(_Current)->GetNext();
			return *this;
		}

		_MyT operator--()
		{
			_Current = AllocatorType::Parse(_Current)->GetNext();
			return *this;
		}

		Element& operator*()
		{
			return AllocatorType::Parse(_Current)->GetElement();
		}

		const Element& operator*() const
		{
			return AllocatorType::Parse(_Current)->GetElement();
		}

		Element* operator->()
		{
			return &(AllocatorType::Parse(_Current)->GetElement());
		}

		bool operator!=(const _MyT& other)
		{
			return _Current != other._Current;
		}

		_INLINE Handle GetNode() const
		{
			return _Current;
		}


	private:
		Handle _Current;
	}; // LinkedListRevereseIterator

	
	template < class T, class AllocatorType >
	class DoubleLinkedList
	{

	public:

		// Typedefs
		
		typedef T ElementType;

		// Typedef handle type from the allocator type
		typedef typename AllocatorType::Handle Handle;

		// Node type
		typedef DoubleLinkedListNode< ElementType, Handle > Node;

		// Rebind allocator type
		//typedef AllocatorType:: template Rebind<Node>:: template Other Allocator;
		typedef typename AllocatorType::template Rebind<Node>::Other Allocator;

		typedef DoubleLinkedListIterator < ElementType, Allocator > iterator;
		typedef DoubleLinkedListIterator < ElementType, Allocator > const_iterator;
		typedef DoubleLinkedListRevereseIterator < ElementType, Allocator > reverse_iterator;
		typedef DoubleLinkedListRevereseIterator < ElementType, Allocator > const_reverse_iterator;

		// Find results defs
		typedef Pair<bool, const_iterator> Result;
		typedef Pair<bool, const_reverse_iterator> rResult;

		// This type
		typedef DoubleLinkedList<T, Allocator> _MyT;

	public:

		// Default constructor
		DoubleLinkedList(Allocator alloc) :
			_size(0),
			allocator(alloc)
		{
			Init();
		};

		// Initialize this list by coping from other container,
		// container should support iterators
		//
		// @param: _container - the other container to fill from
		template < class Container >
		DoubleLinkedList(const Container& _container, Allocator alloc) :
			_size(0),
			allocator(alloc)
		{
			Init();

			// Iterate throw container, should support iteration.
			auto it = _container.CreateConstIterator();
			for (; it != _container.cEnd(); ++it)
				Add(*it);
		}

		// Destructor
		~DoubleLinkedList()
		{
			Clear();

			allocator.Deallocate(_head);
			allocator.Deallocate(_tail);
		}

		// Adds element to the front of the list
		//
		// @param: element - reference to element to add
		// @return: Iterator to inserted location
		const_iterator AddFront(const ElementType& element)
		{
			return _Insert(element, _head);
		}

		// Adds element to the front of the list
		// Move temp
		//
		// @param: element - reference to element to add
		// @return: Iterator to inserted location
		const_iterator AddFront(ElementType&& element)
		{
			return _Insert(_EVEREST Move(element), _head);
		}

		// Adds element to the end of the list
		//
		// @param: element - reference to element to add
		// @return: Iterator to inserted location
		const_iterator Add(const ElementType& element)
		{
			return _Insert(element, Allocator::Parse(_tail)->GetPrev());
		}

		// Adds element to the end of the list
		// Move temp
		//
		// @param: element - reference to element to add
		// @return: Iterator to inserted location
		const_iterator Add(ElementType&& element)
		{
			return _Insert(_EVEREST Move(element), Allocator::Parse(_tail)->GetPrev());
		}

		// Inserts new element node after given node
		// if after node is null or tail, add to end of list,
		// if after node is head, add to front of list
		//
		// @param: element - reference to element to add
		// @param: after - Iterator to insert after
		// @return: Iterator to inserted location
		_INLINE const_iterator Insert(const ElementType& element, const_iterator after)
		{
			return _Insert(element, after.GetNode());
		}

		// Inserts new element node after given node
		// if after node is null or tail, add to end of list,
		// if after node is head, add to front of list
		// Move Temp version.
		//
		// @param: element - reference to element to add
		// @param: after - Iterator to insert after, if null, insertion will be at end
		// @return: Iterator to inserted location
		_INLINE const_iterator Insert(ElementType&& element, const_iterator& after)
		{
			return _Insert(_EVEREST Move(element), after.GetNode());
		}

		// Remove an element's node from the list 
		//
		// @param: element - element to the remove it's node
		void Remove(const ElementType& element)
		{
			if (_size <= 0)
				return;

			// Find the node and remove it
			Remove(Find(element)._second);
		}

		// Remove a node from the list using iterator
		// This is method is faster that Remove(ElementType)
		//
		// @param: element - iterator to the element 
		void Remove(iterator it)
		{
			RemoveNode(it.GetNode());
		}

		// Remove a node from the list 
		//
		// @param: node - node to the remove
		void RemoveNode(Handle node)
		{
			if (node == _head || node == _tail)
				return;

			// Unlink node
			Allocator::Parse(Allocator::Parse(node)->GetPrev())->SetNext(Allocator::Parse(node)->GetNext());
			Allocator::Parse(Allocator::Parse(node)->GetNext())->SetPrev(Allocator::Parse(node)->GetPrev());

			ReleaseNode(node);
		
			_size--;
		}

		// Remove all nodes and destruct them
		void Clear()
		{
			Handle temp = Allocator::Parse(_head)->GetNext();
			while (temp != _tail)
			{
				auto next = Allocator::Parse(temp)->GetNext();
				RemoveNode(temp);
				temp = next;
			}
		}

		// Finds an element start from the begin of the list
		// 
		// @param: element - element to find it's node
		// @return: Pair of found flag and iterator to found item
		
		Result Find(const ElementType& element)
		{
			auto it = cBegin();
			while (it != cEnd())
			{
				if (*it == element)
					return Result(true, it);

				++it;
			}

			return Result(false, cEnd());
		}

		// Finds an element start from the end of the list
		// 
		// @param: element - element to find it's node
		// @return: Pair of found flag and reverese iterator to found item
		rResult FindLast(const ElementType& element)
		{
			auto it = crBegin();
			while (it != crEnd())
			{
				if (*it == element)
					return rResult(true, it);

				++it;
			}

			return rResult(true, crEnd());
		}

		// Return true if the list is empty
		_INLINE bool Empty()
		{
			return _size == 0;
		}

		// Check if this list containes an element
		//
		// @param: element - The element to search
		// @return: true if found
		_INLINE bool Contains(const ElementType& element)
		{
			return Find(element)._first;
		}

		//TODO: test when complete memory manager cuz of alignment
		template <typename Container>
		void ToArray(_out_ref_ Container& array)
		{
			Handle node = Allocator::Parse(_head)->GetNext();
			while (node != _tail)
			{
				array.Add(Allocator::Parse(node)->GetElement());
				node = Allocator::Parse(node)->GetNext();
			}
		}

		// Return a reference to list head
		_INLINE Handle& Head() const
		{
			return _head;
		}

		// Return a reference to list tail
		_INLINE Handle& Tail() const
		{
			return _tail;
		}

		// Return the list size
		_INLINE uint32 Size() const
		{
			return _size;
		}

		// Returns reference to the first element from List
		// Stack method
		//
		// @return: reference element
		ElementType& Top()
		{
			return Allocator::Parse(Allocator::Parse(_head)->GetNext())->GetElement();
		}

		// Returns const reference to the first element from List
		// Stack method
		//
		// @return: const reference element
		const ElementType& Top() const
		{
			return (const ElementType&)Allocator::Parse(Allocator::Parse(_head)->GetNext())->GetElement();
		}

		// Add element to the stack
		// Stack method
		//
		// @param: element - const reference to element to add
		// @return: Iterator to inserted location
		const_iterator Push(const ElementType& element)
		{
			return Add(element);
		}

		// Add element to the stack, by move temp
		// Stack method
		//
		// @param: element - rvalue to element to add
		// @return: Iterator to inserted location
		const_iterator Push(ElementType&& element)
		{
			return Add(_EVEREST Move(element));
		}

		// Remove the last in element of the stack
		// Stack method
		void Pop()
		{
			Remove(Allocator::Parse(_tail)->GetPrev());
		}

		// Returns reference to the front element from List
		// Queue method
		//
		// @return: reference element
		_INLINE ElementType& Front()
		{
			return Top();
		}

		// Returns const reference to the front element from List
		// Queue method
		//
		// @return: const reference element
		_INLINE const ElementType& Front() const
		{
			return Top();
		}

		// Insert an element at the front of the list
		// Queue method
		//
		// @param: element - reference to element to insert
		// @return: Iterator to inserted location
		const_iterator Enqueue(const ElementType& element)
		{
			return Add(element);
		}

		// Insert an element at the front of the list, Move temp
		// Queue method
		//
		// @param: element - reference to element to insert
		// @return: Iterator to inserted location
		const_iterator Enqueue(const ElementType&& element)
		{
			return Add(_EVEREST Move(element));
		}

		// Remove an element from the front of the list
		// Queue method
		void Dequeue()
		{
			Remove(Allocator::Parse(_head)->GetPrev());
		}

		// [DEPRECATED]
		// Allocated free, ready nodes that are sequanced and connceted
		// This is useful if you, sometimes know how many nodes you need
		//
		// @param: count(1) - the count of noded to create
		/*void Reserve(uint32 count = 1)
		{
			if (count <= 0)
				return;

			if (_tempFree.IsNull())
			{
				_tempFree = EMemory::_NEW<Node>();
				--count;
				++_free;
			}

			while (count--)
			{
				pointer node = EMemory::_NEW<Node>();
				node->SetPrev(_tempFree);
				_tempFree->SetNext(node);
				_tempFree = node;

				++_free;
			}
		}*/

		// Return an iterator start from the begining of the list
		//
		// This is the node after head
		_INLINE iterator CreateIterator()
		{
			return Begin();
		}

		// Return a const iterator start from the begining of the list
		//
		// This is the node after head
		_INLINE const_iterator CreateConstIterator() const
		{
			return cBegin();
		}

		// Return a reverese iterator start from the begining of the list
		//
		// This is the node after head
		_INLINE reverse_iterator CreateReverseIterator()
		{
			return rBegin();
		}

		// Return a const reverese iterator start from the begining of the list
		//
		// This is the node after head
		_INLINE const_reverse_iterator CreateConstReverseIterator() const
		{
			return crBegin();
		}

		// Returns an iterator to the begin of the list
		// This is the node after head
		iterator Begin()
		{
			return iterator(Allocator::Parse(_head)->GetNext());
		}

		// Returns a const iterator to the begin of the list
		// This is the node after head
		const_iterator cBegin() const
		{
			return const_iterator(Allocator::Parse(_head)->GetNext());
		}

		// Returns an iterator to the end of the list
		// This is the tail node
		iterator End()
		{
			return iterator(_tail);
		}

		// Returns a const iterator to the end of the list
		// This is the tail node
		const_iterator cEnd() const
		{
			return  const_iterator(_tail);
		}

		// Returns a reverse iterator, start from the end of the list
		// This is the node before tail
		reverse_iterator rBegin()
		{
			return reverse_iterator(Allocator::Parse(_tail)->GetPrev());
		}

		// Returns a const reverse iterator, start from the end of the list
		// This is the node before tail
		const_reverse_iterator crBegin() const
		{
			return const_reverse_iterator(Allocator::Parse(_tail)->GetPrev());
		}

		// Returns a reverse iterator to the end of the list
		// This is the head node
		reverse_iterator rEnd()
		{
			return reverse_iterator(_head);
		}

		// Returns a const reverse iterator to the end of the list
		// This is the head node
		const_reverse_iterator crEnd() const
		{
			return const_reverse_iterator(_head);
		}

	protected:

		const_iterator _Insert(const ElementType& element, Handle afterNode)
		{
			// Check if after node is null or equal list head, and add element at end
			if (afterNode == _tail)
				afterNode = Allocator::Parse(_tail)->GetPrev();

			// create new node next to after node
			Handle node = allocator.Allocate();

			// Link node
			Allocator::Parse(node)->ConstuctElement(element);
			Allocator::Parse(node)->SetPrev(afterNode);
			Allocator::Parse(node)->SetNext(Allocator::Parse(afterNode)->GetNext());
			Allocator::Parse(Allocator::Parse(afterNode)->GetNext())->SetPrev(node);
			Allocator::Parse(afterNode)->SetNext(node);

			// Update size
			_size++;

			return const_iterator(node);
		}

		// Initialize list head & tail
		_INLINE void Init()
		{
			// Init allocator instance
			allocator.Init();

			_head = allocator.Allocate();
			_tail = allocator.Allocate();

			Allocator::Parse(_head)->SetNext(_tail);
			Allocator::Parse(_tail)->SetPrev(_head);
		}

		// TODO: test free node algho
		// Create node by checking free nodes, or create new node
		/*_INLINE pointer NewNode()
		{
			pointer node = nullptr;
			if (_free-- > 0)
			{
				node = _tempFree;
				
				if (_free == 1)
					;
				else if(_free == 0)
					_tempFree = nullptr;
				else if (_free > 1)
					_tempFree = _tempFree->GetPrev();
			}
			else
			{
				node = EMemory::_NEW<Node>();
			}
			
			return node;
		}*/

		_INLINE void ReleaseNode(const Handle& node)
		{
			Allocator::Parse(node)->DestructElement();
			allocator.Deallocate(node);
		}

	private:

		// The list head node, empty node
		Handle _head;

		// The list tail node, empty node
		Handle _tail;

		// The size of the filled nodes in the list
		// This is not including free nodes
		uint32 _size;

		// Allocator instance
		Allocator allocator;

	};


	//TODO: sorted list

	template <class _Key, class _Comparer>
	class SortedListTraits
	{
	public:

		typedef _Key KeyType;
		typedef _Key Value;
		typedef  _Key ValueType;
		typedef _Comparer KeyCompare;

		// Return the key object of the given map entry
		struct _KeyOf
		{
			//template<typename _Ty1, typename _Ty2>
			_INLINE KeyType& operator() (ValueType& value)
			{
				return value;
			}

		};
		_KeyOf KeyOf;

		// Return the value object of the given map entry
		struct _ValueOf
		{
			//template<typename _Ty1, typename _Ty2>
			_INLINE Value& operator() (ValueType& value)
			{
				return value;
			}

		};
		_ValueOf ValueOf;

		// Compare 2 key and return true if value1 is greater than value2
		struct _Compare
		{
			_INLINE int16 operator() (const KeyType& key1, const KeyType& key2)
			{
				if (key1 > key2)
					return 1;
				else if (key1 < key2)
					return -1;
				else
					return 0;
				//return _CompareObj(key1, key2);
			}

			// Key comparer object
			//KeyCompare _CompareObj;
		};
		_Compare Compare;

		// Compare 2 key and return true if they are equal
		struct _Equals
		{
			_INLINE bool operator() (const KeyType& key1, const KeyType& key2)
			{
				return key1 == key2;
			}
		};
		_Equals Equals;
	};

	// SortedList class with Key
	// This class use Red Black Tree for data operations
	// Insert, remove, find are all log(n) 
	// Keys should work with < > = operators
	/*template <class Key,
	class Comparer = Greater<Key>> //TODO: Compared keys SHOULD contains the ><= operators
	class SortedList
		: public Tree<SortedListTraits<Key, Comparer>, Everest::Memory>
	{
	};*/
}