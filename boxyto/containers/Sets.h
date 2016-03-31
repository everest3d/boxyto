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

	//TODO: test set fucntions

	// Set method, return the intersection between to sets
	// ( A and B )
	template <class Container>
	_INLINE Container
		Intersection(const Container& c1, const Container& c2)
	{
		Container result;
		auto it = c1.CreateIterator();
		while (it != c1.End())
		{
			if (c2.Contains(*it))
				result.Add(*it);

			++it;
		}

		return result;
	}

	// Set method, return the union between to sets
	// ( A OR B )
	template <class Container>
	_INLINE Container
		Union(const Container& c1, const Container& c2)
	{
		Container result;
		auto it = c1.CreateIterator();
		while (it != c1.End())
		{
			if (!result.Contains(*it))
				result.Add(*it);

			++it;
		}

		it = c2.CreateIterator();
		while (it != c2.End())
		{
			if (!result.Contains(*it))
				result.Add(*it);

			++it;
		}
	}

	// Set method, return the differnce (complement) between to sets
	// ( A OR B )
	template <class Container>
	_INLINE Container
		Differnce(const Container& c1, const Container& c2)
	{
		Container result;
		auto it = c1.CreateIterator();
		while (it != c1.End())
		{
			if (!c2.Contains(*it))
				result.Add(*it);

			++it;
		}

		return result;
	}

	// Set method, return true if B elements all in A
	// ( A includes B )
	template <class Container>
	_INLINE bool
		Include(const Container& c1, const Container& c2)
	{
		bool result = true;
		auto it = c1.CreateIterator();
		while (it != c1.End())
		{
			if (!c2.Contains)
			{
				result = false;
				break;
			}
		}

		return result;
	}
}