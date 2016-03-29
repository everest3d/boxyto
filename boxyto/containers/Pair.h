#pragma once

#include "../Template/Common.h"
#include "../system.h"

namespace Everest
{
	template<class T1, class T2>
	class Pair
	{
	public:

		typedef T1 First_T;
		typedef T2 Second_T;
		typedef Pair<T1, T2> _MyT;

		// Default constructor
		Pair() :
			_first(),
			_second()
		{};

		Pair(const Pair&) = default;
		Pair(Pair&&) = default;

		// Construct from references
		//Pair(First_T& type1, Second_T& type2) :
		//	_first(type1), _second(type2)
		//{}

		// Construct from const references
		Pair(const First_T& type1, const Second_T& type2) :
			_first(type1), _second(type2)
		{}

		// Move r-value constructor
		Pair(First_T&& type1, Second_T&& type2) :
			_first(_EVEREST Forward<First_T>(type1)),
			_second(_EVEREST Forward<Second_T>(type2))
		{}


		//template<class Type1, class Type2>
		//Pair(Pair<Type1, Type2>&& _other)
		//{}

		template <class Other1, class Other2>
		_INLINE _MyT&
			operator= (Pair<Other1, Other2>&& _right)
		{
			_first = _EVEREST Forward<Other1>(_right._first);
			_second = _EVEREST Forward<Other2>(_right._second);
			return (*this);
		}

		_INLINE _MyT&
			operator= (const _MyT& _right)
		{
			_first = _right._first;
			_second = _right._second;
			return (*this);
		}

		_INLINE _MyT&
			operator= (Pair&& _right)
		{
			_first = _EVEREST Forward<First_T>(_right._first);
			_second = _EVEREST Forward<Second_T>(_right._second);
			return (*this);
		}

		_INLINE bool
			operator== (const Pair& other)
		{
			return (other._first == _first && other._second == _second);
		}

		_INLINE bool
			operator!= (const Pair& other)
		{
			return (other._first != _first || other._second != _second);
		}

		First_T _first;
		Second_T _second;
	};

	// Pair simple creation 
	//
	// Example:
	// auto myPair = MakePair(4, 2.5f);
	template < class T1, class T2 >
	_INLINE Pair < T1, T2 >
		MakePair(T1&& first, T2&& second)
	{
		typedef Pair < T1, T2 > _MyPair;
		return _MyPair(_EVEREST Forward<T1>(first),
			_EVEREST Forward<T2>(second));
	}
}