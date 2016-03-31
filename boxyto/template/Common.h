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

// template_support describtion:
//
// Because many std lib functions might have overhead
// I descided to create some of the most critical functions
// related to templates.
//

namespace Everest
{
	// std::enable_if like
	template<bool Test, class T = void>
	struct EnableIf
	{
		// False case
	};
	template <class T>
	struct EnableIf<true, T>
	{
		typedef T Type;
	};

	// Select type based on condition bool
	template <bool Test, class T1, class T2>
	struct SelectType
	{
		typedef T2 Type;
	};
	template <class T1, class T2>
	struct SelectType<true, T1, T2>
	{
		typedef T1 Type;
	};

	// RemoveReference struct
	template < class T >
	struct RemoveReference
	{
		typedef T type;
	};
	template < class T >
	struct RemoveReference<T&>
	{
		typedef T type;
	};
	template < class T >
	struct RemoveReference<T&&>
	{
		typedef T type;
	};

	// perfect Forward just like std::forward, but with no overhead
	template < class T >
	_INLINE T&&
		Forward(typename RemoveReference<T>::type& arg)
	{
		return (T&&)arg;
	}

	// perfect Forward just like std::forward, but with no overhead
	template < class T >
	_INLINE T&&
		Forward(typename RemoveReference<T>::type&& arg)
	{
		return (T&&)arg;
	}

	// Temp move template function, just like std::move, , but with no overhead
	template < class T >
	_INLINE typename RemoveReference<T>::type&&
		Move(T&& arg)
	{
		return (typename RemoveReference<T>::type&&)arg;
	}

	// Check an integral-type's bit status
	//
	// @param: val - The integral value to check
	// @param: position - bit position in val
	// @return: True if bit is set
	template <typename T>
	_INLINE bool CheckBit(const T& val, int8 position)
	{
		return (val & (1 << position)) != 0;
	}

	// Set a bit in an integral-type
	//
	// @param: val - The integral value
	// @param: position - bit position in val
	// @return: new modified val
	template <typename T>
	_INLINE T SetBit(T& val, int8 position)
	{
		return val | (1 << position);
	}

	// Clear a bit in an integral-type
	//
	// @param: val - The integral value
	// @param: position - bit position in val
	// @return: new modified val
	template <typename T>
	_INLINE T ClearBit(T& val, int8 position)
	{
		return val & ~(1 << position);
	}

	// Toggle a bit in an integral-type
	//
	// @param: val - The integral value
	// @param: position - bit position in val
	// @return: new modified val
	template <typename T>
	_INLINE T ToggleBit(T& val, int8 position)
	{
		return val ^ (1 << position);
	}
}


