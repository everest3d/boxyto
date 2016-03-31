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
#include "Pair.h"
//#include "Hash.h"
#include "../Template/Common.h"
#include "../Template/Compare.h"
#include "Tree.h"

namespace Everest
{

	template <class _Key, class _Value>
	class MapTraits
	{
	public:

		typedef _Key KeyType;
		typedef _Value Value;
		typedef Pair<_Key, Value> ValueType;

		// Return the key object of the given map entry
		struct _KeyOf
		{
			//template<typename _Ty1, typename _Ty2>
			_INLINE KeyType& operator() (ValueType& value)
			{
				return value._first;
			}

		};
		_KeyOf KeyOf;

		// Return the value object of the given map entry
		struct _ValueOf
		{
			//template<typename _Ty1, typename _Ty2>
			_INLINE Value& operator() (ValueType& value)
			{
				return value._second;
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

	// Map class with key value pair
	// This class use Red Black Tree for data operations
	// Insert, remove, find are all log(n) 
	// Keys should work with < > = operators
	template <class Key, 
		class Value,
		class AllocatorType
		> //TODO: Compared keys SHOULD contains the ><= operators
	class Map 
		: public Tree<MapTraits<Key, Value>, AllocatorType>
	{
	public:

		// Typedefs

		typedef Tree<MapTraits<Key, Value>, AllocatorType> MyBase;

		Map(AllocatorType alloc) :
			MyBase(alloc)
		{}
	};

	//...
	/*template <class Key,
	class Value,
	class Comparer = Greater<Key >>
	class HashMap
		: public Hash <MapTraits<Key, Value, Comparer>>
	{};*/
		
}