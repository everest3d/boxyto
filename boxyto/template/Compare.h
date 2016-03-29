#pragma once

#include "../system.h"

namespace Everest
{
	template <class T>
	struct EqualTo
	{
		// Tests if two values of the same type are equal
		_CONST_FUNC bool operator() (const T& _ty1, const T& _ty2) const
		{
			return (_ty1 == _ty2);
		}
	};

	template <class T>
	struct Greater
	{
		// Tests if first value is greater than second value
		_CONST_FUNC bool operator() (const T& _ty1, const T& _ty2) const
		{
			return (_ty1 > _ty2);
		}
	};

	template <class T>
	struct Less
	{
		// Tests if first value is less than second value
		_CONST_FUNC bool operator() (const T& _ty1, const T& _ty2) const
		{
			return (_ty1 < _ty2);
		}
	};
}