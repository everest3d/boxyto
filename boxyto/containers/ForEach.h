#pragma once

#include "Iterators.h"
#include "../system.h"

namespace Everest
{

	// For Each loop that loop using container's iterators
	// 
	// @param: first - start iterator
	// @param: last - finish iterator
	// @param: func - function object to execute for each item
	// func signature should be always void func([const] ElementType& element)
	template < class It, class Func >
	_INLINE Func 
		ForEach(It first, It last, Func func)
	{
		for (; first != last; ++first)
			func(*first);

		return _EVEREST Move(func);
	}
}