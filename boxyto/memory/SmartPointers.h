#pragma once

#include "Memory.h"

namespace Everest
{
	template < class T >
	class SmartPointerBase
	{
	public:
		// Operator for equality check with other pointer,
		// two pointers are equal if thier IDs & _Pointers are equal
		_INLINE const bool operator== (const SmartPointerBase& _other) const
		{
			return (_pointer == _other._pointer);
		}

		template <typename _OtherType>
		// Operator for equality check with other pointer,
		// two pointers are equal if thier IDs & _Pointers are equal
		_INLINE const bool operator== (const SmartPointerBase<_OtherType>& _other) const
		{
			return (_pointer == _other._pointer);
		}

		// Operator check if this pointer is not equal with other pointer,
		// two pointers are not equal if thier IDs & _Pointers are not equal
		_INLINE bool operator!= (const SmartPointerBase& _other)
		{
			return  (_pointer != _other._pointer);
		}

		// pointer access operator
		_INLINE T* operator->()
		{
			return _pointer;
		}

		// pointer access operator
		_INLINE const T* operator->() const
		{
			return _pointer;
		}

		// Return the pointer object
		_INLINE T* Get()
		{
			return _pointer;
		}

		// Return the pointer object
		_INLINE const T* Get() const
		{
			return _pointer;
		}

	protected:
		T* _pointer;
	};

	template < class T >
	class UniquePointer :
		public SmartPointerBase < T >
	{

		// friend pointer base class
		friend class SmartPointerBase<T>;

	public:

		// Typedefs
		typedef UniquePointer < T > _Myt;
		typedef SmartPointerBase<T> _Base;
		typedef T ElementType;
		typedef ElementType* pointer;

		explicit UniquePointer() :
			_Base()
		{}

		// Construct from a pointer
		UniquePointer(pointer p) :
			_Base(p)
		{}

		// Construct by ownership transfere
		UniquePointer(UniquePointer&& other) :
			_Base(other.Release())
		{}

		// Construct by ownership transfere
		UniquePointer(UniquePointer& other) :
			_Base(other.Release())
		{}

		// Delete the pointer as UniquePointer went out of scope
		~UniquePointer()
		{
			if (Get())
			{
				Get()->~ElementType();
				_DELETE(Get());
			}
		}

	}; // UniquePointer

	   // Shared pointer class, with reference monitoring support
	template < class T >
	class SharedPointer :
		public SmartPointerBase < T >
	{

		// friend pointer base class
		friend class SmartPointerBase<T>;

	public:

		// Typedefs
		typedef SharedPointer < T > _Myt;
		typedef SmartPointerBase<T> _Base;
		typedef T ElementType;
		typedef ElementType* pointer;

		// Default constructor
		SharedPointer() :
			_Base(), _refCount(nullptr)
		{}

		// Construct and initialize new shared pointer
		SharedPointer(pointer p) :
			_Base(p)
		{
			_refCount = new RefCount;
			_refCount->IncRef();
		}

		// Construct by incementing ref count 
		template <typename _OtherT>
		SharedPointer(SharedPointer<_OtherT>& _other) : _Base(_other.Get())
		{
			_refCount = _other._refCount;
			_refCount->IncRef();
		}

		// Construct by incementing ref count
		SharedPointer(_Myt& _other)
			: _Base(_other.Get())
		{
			_refCount = _other._refCount;
			_refCount->IncRef();
		}

		// Destruct when no reference
		~SharedPointer()
		{
			if (_refCount->DecRef() == 0)
				Reset();
		}

		// Equal operator, move ownership to this from other
		_INLINE _Myt& operator= (_Myt& _other)
		{
			if (this != &_other)
			{
				if (_refCount.DecRef() == 0)
					Reset();

				Get() = _other.Get();
				_refCount = _other._refCount;
			}

			return (*this);
		}

		// Equal operator, move ownership to this from other
		template<typename Other_T>
		_INLINE SharedPointer<Other_T>
			operator= (const SharedPointer<Other_T>& _other)
		{
			if (this != &_other)
			{
				if (_refCount->DecRef() == 0)
					Reset();

				Get() = _other.Get();
				_refCount = _other._refCount;
			}

			return (*this);
		}

	private:

		// Class used to handle shadred pointer reference count
		class RefCount
		{
		public:

			// Constructor, sets count to 0
			RefCount() : _count(0)
			{}

			// Increment count and return new count
			_INLINE uint32 IncRef()
			{
				return ++_count;
			}

			// Decrement count and return new count
			_INLINE uint32 DecRef()
			{
				return --_count;
			}

			// Return the current references count
			_INLINE uint32 Count()
			{
				return _count;
			}

		private:

			// count of pointer references
			uint32 _count;

		}; // RefCount

		   // The reference count object
		RefCount* _refCount;

		_INLINE void Reset()
		{
			delete _refCount;

			// Destruct & delete
			Get()->~ElementType();
			_DELETE(Get());
		}
	}; // SharedPointer
}