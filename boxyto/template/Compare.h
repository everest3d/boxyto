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