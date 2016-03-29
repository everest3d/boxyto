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