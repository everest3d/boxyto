#pragma once

#include "../system.h"
#include "../Template/Common.h"
#include "List.h"
#include "Array.h"
#include "../memory/LinearAllocator.h"
#include "../memory/SimpleNodeAllocator.h"

namespace Everest
{

	template<class _Traits>
	class Hash
	{
	public:

		// Typedefs
		typedef Hash<_Traits> _MyT;

		typedef typename _Traits::ValueType ValueType;
		typedef typename _Traits::KeyType KeyType;
		typedef typename _Traits::Value Value;

		typedef typename DoubleLinkedList<ValueType, SimpleNodeAllocator<ValueType>> BucketType;
		typedef typename Array<BucketType, LinearAllocator<BucketType>> Buckets;

		// Init with default params
		Hash() :
			_capacity(8), // Default capacity
			_loadFactor(.75f), // Default load factor
			_size(0)
		{
			_buckets.ConstructEmpty(0, _capacity);
		};

		// Init with capacity and default load factor
		Hash(uint32 initCapacity) :
			_capacity(initCapacity),
			_loadFactor(.75f),
			_size(0)
		{
			_buckets.ConstructEmpty(0, _capacity);
		}

		// Init with capacity and loadFactor
		// load factor must be in the range if .1 and 1
		Hash(uint32 initCapacity, float loadFactor) :
			_initCapacity(initCapacity)
		{
			_loadFactor = (loadFactor > 1.0f || loadFactor < .1f) ? .75f : loadFactor;
			_buckets.ConstructEmpty(0, _capacity);
		}

		~Hash()
		{
			Reset();
		}

		// Rehash the hole table, 
		// don't call this method until you know how to use
		void Rehash()
		{
			uint32 bucket = 0;

			// Create the new hash table
			Buckets newTable;
			newTable.ConstructEmpty(0, _capacity);

			// Move elements to the new table
			auto first = _buckets.CreateIterator();
			while (first != _buckets.End()) // Buckets loop
			{
				auto bFirst = (*first).CreateIterator();
				while (bFirst != (*first).End()) // Elements loop
				{
					uint64 hash = _Hash(_traits.KeyOf(*bFirst)) % _capacity;
					newTable[(int32)hash].Add(_EVEREST Move(*bFirst));
					++bFirst;
				} // Elements loop 

				++first;
			} // Buckets loop

			_buckets = newTable;
		}

		// Copying add reference element , Move Temp
		// 
		// @param: element - rererence element to copy
		// return: index of the added element
		_INLINE void Add(ValueType&& element)
		{
			Insert(_EVEREST Move(element));
		}

		// Insert an element at hashed bucket
		// 
		// @param: element - The element to copy and insert
		void Insert(ValueType&& element)
		{
			CheckSize();
			uint32 hash = _Hash(_traits.KeyOf(element)) % _capacity;
			_buckets[hash].Add(element);
			++_size;
		}

		// Insert a unique element at hashed bucket
		// If element exsit, nothing added
		// 
		// @param: element - The element to copy and insert
		void InsertUnique(ValueType&& element)
		{
			if (Find(_traits.KeyOf(element)) != nullptr)
				return;

			CheckSize();
			uint32 hash = _Hash(_traits.KeyOf(element)) % _capacity;
			_buckets[hash].Add(element);
			++_size;
		}

		// Remove elements(s) that matches key
		//
		// @param: key - the key to remove
		// @return: count of items removed
		uint32 Remove(const KeyType& key)
		{
			uint32 hash = _Hash(key) % _capacity;
			if (hash >= _buckets.Size())
				return 0;

			int32 removed = 0;
			auto& bucket = _buckets[hash];
			auto first = bucket.CreateIterator();
			while (first != bucket.End())
			{
				//auto temp = first++;
				auto comp = _traits.Compare(key, _traits.KeyOf(*first));
				if (comp == 0)
					bucket.Remove(first++);
				else
					++first;

				//first = temp;
				++removed;
			}
			_size -= removed;
			return removed;
		}

		// Replace exsiting elements at some key with value
		// 
		// @param: value - the value to update
		// @return: Count updated
		uint32 Replace(const KeyType& key, Value& value)
		{
			uint32 hash = _Hash(key) % _capacity;
			if (!_buckets.ContainIndex(hash))
				return 0;

			uint32 count = 0;
			auto bucket = _buckets[hash];
			auto first = bucket.CreateIterator();
			while (first != bucket.End()) // Elements loop
			{
				auto comp = _traits.Compare(key, _traits.KeyOf(*first));
				if (comp == 0)
				{
					new(&_traits.ValueOf(*first)) Value(value);
					++count;
				}

				++first;
			} // Elements loop 

			++first;

			return count;
		}

		// Replace all elements in this tree with value
		// 
		// @param: value - the value to update
		// @return: Count updated
		uint32 ReplaceAll(Value& value)
		{
			uint32 count = 0;

			auto first = _buckets.CreateIterator();
			while (first != _buckets.End()) // Buckets loop
			{
				auto bFirst = (*first).CreateIterator();
				while (bFirst != (*first).End()) // Elements loop
				{
					new(&_traits.ValueOf(*bFirst)) Value(value);
					++count;
					++bFirst;
				} // Elements loop 

				++first;
			} // Buckets loop

			return count;
		}

		// Execute Func function for all elements in the hash
		template <class Func>
		Func For_Each (Func _func)
		{
			auto first = _buckets.CreateIterator();
			while (first != _buckets.End()) // Buckets loop
			{
				auto bFirst = (*first).CreateIterator();
				while (bFirst != (*first).End()) // Elements loop
				{
					_func(*bFirst);
					++bFirst;
				} // Elements loop 

				++first;
			} // Buckets loop

			return _EVEREST Move(_func);
		}

		// Return count of elements with a given key
		//
		// @param: key - The key
		// @param: found elements count
		uint32 Count(const KeyType& key)
		{
			uint32 count = 0;
			int32 hash = _Hash(key) % _capacity;
			if (_buckets.ContainIndex(hash))
			{
				auto& bucket = _buckets[hash];
				auto first = bucket.CreateIterator();
				while (first != bucket.End())
				{
					auto comp = _traits.Compare(key, _traits.KeyOf(*first));
					if (comp == 0)
						++count;

					++first;
				}
			}

			return count;
		}

		// Find, from front to back, an element by it's key, 
		// the first element found will be consedered
		//
		// @param: key: the key to find it's value
		// @return: pointer to found element, or nullptr if not found
		Value* Find(const KeyType& key)
		{
			int32 hash = _Hash(key) % _capacity;
			if (_buckets.ContainIndex(hash))
			{
				auto& bucket = _buckets[hash];
				auto first = bucket.CreateIterator();
				while (first != bucket.End())
				{
					auto comp = _traits.Compare(key, _traits.KeyOf(*first));
					if (comp == 0)
						return &(_traits.ValueOf(*first));

					first++;
				}
			}

			return nullptr;
		}

		// Find, from front to back, an element by it's key, 
		// the first element found will be consedered
		//
		// @param: key: the key to find it's value
		// @return: pointer to found element, or nullptr if not found
		const Value* Find(const KeyType& key) const
		{
			int32 hash = _Hash(key) % _capacity;
			if (_buckets.ContainIndex(hash))
			{
				auto& bucket = _buckets[hash];
				auto first = bucket.CreateIterator();
				while (first != bucket.End())
				{
					auto comp = _traits.Compare(key, _traits.KeyOf(*first));
					if (comp == 0)
						return const_cast<Hash*>(this)->Find(key);

					first++;
				}
			}

			return nullptr;
		}

		// Copy all keys in array 
		//
		// @param: keys - ref Array to fill with keys
		void GetKeys(_out_ref_ Array<KeyType>& keys)
		{
			auto first = _buckets.CreateIterator();
			while (first != _buckets.End()) // Buckets loop
			{
				auto bFirst = (*first).CreateIterator();
				while (bFirst != (*first).End()) // Elements loop
				{
					keys.Add(_traits.KeyOf(*bFirst));
					++bFirst;

				} // Elements loop 

				++first;
			} // Buckets loop
		}

		// Copy UNIQUE keys in array 
		//
		// @param: keys - ref Array to fill with keys
		void GetUniqueKeys(_out_ref_ Array<KeyType>& keys)
		{
			auto first = _buckets.CreateIterator();
			while (first != _buckets.End()) // Buckets loop
			{
				auto bFirst = (*first).CreateIterator();
				while (bFirst != (*first).End()) // Elements loop
				{
					auto& key = _traits.KeyOf(*bFirst);
					if (!keys.Contains(key))
						keys.Add(key);
					++bFirst;

				} // Elements loop 

				++first;
			} // Buckets loop

			keys.ShrinkToFit();
		}

		// Copy all values in array 
		//
		// @param: values - ref Array to fill with values
		void GetValues(_out_ref_ Array<Value>& values)
		{
			auto first = _buckets.CreateIterator();
			while (first != _buckets.End()) // Buckets loop
			{
				auto bFirst = (*first).CreateIterator();
				while (bFirst != (*first).End()) // Elements loop
				{
					values.Add(_traits.ValueOf(*bFirst));
					++bFirst;

				} // Elements loop 

				++first;
			} // Buckets loop
		}

		// Copy all values in array that match a given key
		//
		// @param: values - ref Array to fill with values
		// @param: key - The key to match
		void GetValues(_out_ref_ Array<Value>& values, const KeyType& key)
		{
			int32 hash = _Hash(key) % _capacity;
			if (_buckets.ContainIndex(hash))
			{
				auto& bucket = _buckets[hash];
				auto first = bucket.CreateIterator();
				while (first != bucket.End())
				{
					auto comp = _traits.Compare(key, _traits.KeyOf(*first));
					if (comp == 0)
						values.Add(_traits.ValueOf(*first));

					++first;
				}
			}
		}

		// Clear all elements from the table
		void Clear()
		{
			Reset();
			_capacity = 8;
			CheckSize();
		}

		// Return the size of the table
		_INLINE const uint32 Size() const
		{
			return _size;
		}

	protected:

		// Ensure there is enought size based on the load factor
		void CheckSize()
		{
			if (((float)_size / (float)_capacity) >= _loadFactor)
			{
				_capacity *= 2;
				//_buckets.ConstructEmpty(_buckets.Size() , _capacity);

				Rehash();
			}
		}

		_INLINE void Reset()
		{
			auto start = _buckets.CreateIterator();
			while (start != _buckets.End())
			{
				(*start).Clear();
				++start;
			}
			_buckets.Clear();
			_capacity = _size = 0;
			_loadFactor = .75f;
		}

	private:

		// The capacity as all allocated buckets
		uint32 _capacity;

		// The growth load factor
		float _loadFactor;

		// Count of current elements in all buckets
		int32 _size;

		// The array of table buckets
		Buckets _buckets;

		// The traits object for other consumers
		_Traits _traits;
	};

	//FNV-1a hash function
	template <typename T>
	_INLINE 
		uint64 _Hash(const T& _element)
	{	
		uint64 _Count = sizeof(T);
		const unsigned char* _toHash = (unsigned char*)&_element;

		const uint64 _FNV_offset_basis = 14695981039346656037ULL;
		const uint64 _FNV_prime = 1099511628211ULL;

		uint64 _Val = _FNV_offset_basis;
		for (uint64 _Next = 0; _Next < _Count; ++_Next)
		{	// fold in another byte
			_Val ^= (uint64)_toHash[_Next];
			_Val *= _FNV_prime;
		}
		return (_Val);
	}

	// TODO: To Add
	// 1- Append

}