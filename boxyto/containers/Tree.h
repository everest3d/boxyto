#pragma once

#include "../system.h"
#include "../Template/Common.h"
#include "../memory/MemoryOps.h"
#include "../memory/Pointer.h"

namespace Everest
{
	
	template <class _Tree>
	class TreeIterator
	{
		//typedefs
		typedef TreeIterator<_Tree> _MyT;
		typedef typename _Tree::ValueType ValueType;
		typedef typename _Tree::NodePointer NodePointer;

	public:
		TreeIterator(NodePointer node,  _Tree* tree) : _myNode(node), _tree(tree)
		{}

		ValueType& operator* ()
		{
			return _myNode->_value;
		}

		ValueType* operator-> ()
		{
			return &_myNode->_value;
		}

		_MyT& operator++ ()
		{
			NodePointer succ = _tree->Successor(_myNode);
			if (!succ.IsNull() || !_tree->IsNil(succ))
			{
				_myNode = succ;
				return *this;
			}
			else
			{
				while (!_tree->IsNil(_myNode->_parent))
				{
					auto comp = _tree->Traits().Compare(
						_tree->Traits().KeyOf(_myNode->_value), _tree->Traits().KeyOf(_myNode->_parent->_value));
					if (comp >= 0)
					{
						_myNode = _myNode->_parent;
						return *this;
					}
				}
			}

			_myNode = _tree->Nil();

			return *this;
		}

		_MyT& operator++ (int)
		{
			_MyT _Tmp = *this;
			++*this;
			return _Tmp;
		}

		_MyT& operator-- ()
		{
			if (_tree->IsNil(_myNode)) // End
			{
				_myNode = _tree->Max(_tree->Root());
				return *this;
			}

			NodePointer pred = _tree->Predecessor(_myNode);
			if (!pred.IsNull() || !_tree->IsNil(pred))
			{
				_myNode = pred;
			}
			else if (!_tree->IsNil(_myNode->_left))
			{
				_myNode = _myNode->_left;
			}
			
			return *this;
		}

		_MyT& operator-- (int)
		{
			_MyT _Tmp = *this;
			--*this;
			return _Tmp;
		}

		bool operator== (const _MyT& other)
		{
			return _myNode == other._myNode;
		}

		bool operator!= (const _MyT& other)
		{
			return !(*this == other);
		}

		NodePointer MyNode()
		{
			return _myNode;
		}

		const NodePointer MyNode() const
		{
			return _myNode;
		}

	private:

		NodePointer _myNode;
		_Tree* _tree;
	};

	template <class _Tree>
	class TreeReverseIterator
	{
		//typedefs
		typedef TreeReverseIterator<_Tree> _MyT;
		typedef typename _Tree::ValueType ValueType;
		typedef typename _Tree::NodePointer NodePointer;

	public:
		TreeReverseIterator(NodePointer node, _Tree* tree) : _myNode(node), _tree(tree)
		{}

		ValueType& operator* () const
		{
			return _myNode->_value;
		}

		ValueType* operator-> () const
		{
			return &_myNode->_value;
		}

		_MyT& operator++ ()
		{
			NodePointer pred = _tree->Predecessor(_myNode);
			if (!pred.IsNull() || !_tree->IsNil(pred))
			{
				_myNode = pred;
			}
			else if (!_tree->IsNil(_myNode->_left))
			{
				_myNode = _myNode->_left;
			}
			else
			{
				_myNode = _tree->Nil();
			}

			return *this;
		}

		_MyT& operator++ (int)
		{
			_MyT _Tmp = *this;
			++*this;
			return _Tmp;
		}

		_MyT& operator-- ()
		{
			NodePointer succ = _tree->Successor(_myNode);
			if (!succ.IsNull() || !_tree->IsNil(succ))
			{
				_myNode = succ;
				return *this;
			}
			else
			{
				while (!_tree->IsNil(_myNode->_parent))
				{
					auto comp = _tree->Traits().Compare(
						_tree->Traits().KeyOf(_myNode->_value), _tree->Traits().KeyOf(_myNode->_parent->_value));
					if (comp >= 0)
					{
						_myNode = _myNode->_parent;
						return *this;
					}
				}
			}

			_myNode = _tree->Nil();

			return *this;
		}

		_MyT& operator-- (int)
		{
			_MyT _Tmp = *this;
			--*this;
			return _Tmp;
		}

		bool operator== (const _MyT& other)
		{
			return _myNode == other._myNode;
		}

		bool operator!= (const _MyT& other)
		{
			return !(*this == other);
		}

		NodePointer MyNode()
		{
			return _myNode;
		}

		const NodePointer MyNode() const
		{
			return _myNode;
		}

	private:

		NodePointer _myNode;
		_Tree* _tree;
	};

	

	// Red-Black tree node
	template< class ValueType, class Parser >
	struct Node_RB
	{
		typedef typename Node_RB<ValueType, Parser> type;
		typedef typename NodePointer<Parser, type> Pointer;

		Pointer _parent; // This node parent
		Pointer _right; // The left node, larger
		Pointer _left; // The right node, smaller
		ValueType _value; // The node's element value 
		uint8 _color; // Red or black node color
		//uint8 _isNil; // True if the node is null or leaf node
	};

	// Red black tree
	template <class _Traits, class AllocatorType>
	class Tree
	{
	public:

		// Red-Black tree colors
		enum RB_TREE_COLORS
		{
			TREE_COLOR_RED = 48, 
			TREE_COLOR_BLACK = 49
		};

		// Typedefs
		typedef typename _Traits::ValueType ValueType;
		typedef typename _Traits::KeyType KeyType;
		typedef typename _Traits::Value Value;

		typedef typename AllocatorType::Parser Parser;
		typedef typename Node_RB<ValueType, Parser> Node;
		typedef typename AllocatorType::template Rebind<Node>::Other Allocator;
		typedef typename Node::Pointer NodePointer;

		typedef Tree<_Traits, Allocator> _MyT;

		typedef TreeIterator<Tree<_Traits, AllocatorType>> iterator;
		typedef TreeReverseIterator<_MyT> reverse_iterator;

		// Find results defs
		typedef Pair<bool, iterator> Result;
		typedef Pair<bool, reverse_iterator> rResult;

		enum NIL_VAL
		{
			_TRUE = 49, _FALSE = 48
		};

		// Pair of node pointer & value position for search results
		//typedef Pair<NodePointer, int32> Result;

		Tree() = delete;

		Tree(Allocator alloc) :
			allocator(alloc),
			_size(0)
		{
			_Init();
		}

		// Construct with other map
		Tree(const _MyT& other) :
			allocator(other.GetAllocator()),
			_size(0)
		{
			_Init();
			Append(other);
		}

		// Construct with other map
		// Move temp version
		Tree(_MyT&& other) :
			allocator(other.GetAllocator()),
			_size(0)
		{
			_Init();
			Append(_EVEREST Move(other));
		}

		// Construct from range
		//Tree(Iterator first, Iterator last)
		//{
		//	while (first != last)
		//		Insert(*first++)
		//}

		// Destructor
		~Tree()
		{
			Clear();

			allocator.Deallocate(_root.GetHandle());

			//_DELETE(_nil.Get());
		}
		
		// Insert new element in the tree
		// Move temp
		//
		// @param: value - Element value to add
		_INLINE NodePointer Add(ValueType&& value)
		{
			return Insert(_EVEREST Move(value));
		}

		// Insert new element in the tree
		//
		// @param: value - The key/value to add 
		// @return: pointer to the inserted node
		_INLINE NodePointer Insert(ValueType&& value)
		{
			NodePointer node = allocator.Allocate();
			new(&node->_value) ValueType(_EVEREST Move(value));
			_Insert(node, _root);
			return node;
		}

		// Insert element if not exsits
		// If element found, then update its value 
		// NOTE: if element is not unique, the first found only updated
		//
		// @param: value - The key/value to add 
		// @return: pointer to the inserted node or the exsit node
		NodePointer InsertUnique(ValueType&& value)
		{
			NodePointer found = _Find(_traits.KeyOf(value));
			if (found.IsNull())
				return Insert(_EVEREST Move(value));
			else 
				new(&found->_value) ValueType(_EVEREST Move(value));

			return found;
		}

		// Replace exsiting elements at some key with value
		// 
		// @param: value - the value to update
		// @return: Count updated
		uint32 Replace(const KeyType& key, Value& value)
		{
			//const KeyType& key = _traits.KeyOf(value);
			auto it = Begin();
			uint32 count = 0;

			for (; it != End(); ++it)
			{
				auto comp = _traits.Compare(_traits.KeyOf(*it), key);
				if (comp == 0)
				{
					new(&_traits.ValueOf(*it)) Value(value);
					++count;
				}
				else if (comp > 0)
				{
					break;
				}
			}

			return count;
		}

		// Replace all elements in this tree with value
		// 
		// @param: value - the value to update
		// @return: Count updated
		uint32 ReplaceAll(Value& value)
		{
			auto it = Begin();
			uint32 count = 0;
			
			for (; it != End(); ++it)
			{
				new(&_traits.ValueOf(*it)) Value(value);
				++count;
			}

			return count;
		}

		// Appends other tree to this tree
		//
		// @param: other - the other tree
		void Append(const _MyT& other)
		{
			auto it = other.Begin();
			for (; it != other.End(); ++it)
				Insert(*it);
		}

		// Appends other tree to this tree
		// Move temp version
		//
		// @param: other - the other tree
		void Append(_MyT&& other)
		{
			auto it = other.Begin();
			for (; it != other.End(); ++it)
				Insert(_EVEREST Move(*it));

			other.Clear();
		}

		/*_MyT& operator= (const _MyT& right)
		{
			if (&right != this)
			{
				Clear();
				Append(right);
			}

			return *this;
		}

		_MyT& operator= (_MyT&& right)
		{
			if (&right != this)
			{
				Clear();
				Append(_EVEREST Move(right));
			}

			return *this;
		}*/

		// Remove a node given key
		//
		// @param: key - The key to remove
		void Remove(const KeyType& key)
		{
			NodePointer z = _Find(key);
			if (z.IsNull())
				return;

			NodePointer x = _nil, y = _nil;

			if ((z->_left == _nil) || (z->_right == _nil))
				y = z;
			else
				y = Successor(z);

			if (y->_left != _nil)
				x = y->_left;
			else
				x = y->_right;

			x->_parent = y->_parent;

			if (y->_parent == _nil)
				_root = x;
			else if ((y->_parent->_left != _nil) && (y->_parent->_left == y))
				y->_parent->_left = x;
			else if ((y->_parent->_right != _nil) && (y->_parent->_right == y))
				y->_parent->_right = x;

			if (y != z)
				z->_value = y->_value;

			if (y->_color == TREE_COLOR_BLACK)
				_RemoveFix(x);

			// free deleted node
			(z->_value).~ValueType();
			allocator.Deallocate(z.GetHandle());
			--_size;
		}

		// TODO: test new optmizations
		// Return the count of nodes contains key
		uint32 Count(const KeyType& key)
		{
			auto it = Begin();
			uint32 count = 0;

			for (; it != End(); ++it)
			{
				auto comp = _traits.Compare(_traits.KeyOf(*it), key);
				if (comp == 0)
					++count;
				else if (comp > 0)
					break;
			}

			return count;
		}

		// Find an element value at key
		// Return pointer to found element or nullptr
		// 
		// @param: The key to find
		// @return: found value
		Value* Find(const KeyType& key)
		{
			NodePointer found = _Find(key);
			if (found.IsNull() || found == Nil())
				return nullptr;
			else
				return &(_traits.ValueOf(found->_value));
		}

		// Find an element value at key
		// Return cosnt pointer to found element or nullptr
		// 
		// @param: The key to find
		// @return: found value as const
		const Value* Find(const KeyType& key) const
		{
			NodePointer found = _Find(key);
			if (found.IsNull() || found == Nil())
				return nullptr;
			else
				return const_cast<Tree*>(this)->Find(key);
		}

		// Check if the tree has some value 
		// return true if found
		bool Contains(const ValueType& value)
		{
			return Find(_traits.KeyOf(value)) != nullptr;
		}

		// Copy all keys in array 
		//
		// @param: keys - ref Array to fill with keys
		template <typename Container>
		void GetKeys(_out_ref_ Container& keys)
		{
			keys.Reserve(_size);
			auto it = Begin();
			for (; it != End(); ++it)
				keys.Add(_traits.KeyOf(*it));
		}

		// Copy UNIQUE keys in array 
		//
		// @param: keys - ref Array to fill with keys
		template <typename Container>
		void GetUniqueKeys(_out_ref_ Container& keys)
		{
			keys.Reserve(_size);
			auto it = Begin();
			for (; it != End(); ++it)
			{
				auto& key = _traits.KeyOf(*it);
				if (!keys.Contains(key))
					keys.Add(key);
			}

			keys.ShrinkToFit();
		}

		// Copy all values in array 
		//
		// @param: values - ref Array to fill with values
		template <typename Container>
		void GetValues(_out_ref_ Container& values)
		{
			keys.Reserve(_size);
			auto it = Begin();
			for (; it != End(); ++it)
				values.Add(_traits.ValueOf(*it));
		}

		// Copy all values in array that match a given key
		//
		// @param: values - ref Array to fill with values
		// @param: key - The key to match
		template <typename Container>
		void GetValues(_out_ref_ Container& values, const KeyType& key)
		{
			keys.Reserve(_size);
			auto it = Begin();
			for (; it != End(); ++it)
			{
				auto comp = _traits.Compare(key, _traits.KeyOf(*it));
				if (comp == 0)
					values.Add(_traits.ValueOf(*it));
				else if (comp > 0)
					break;
			}

			values.ShrinkToFit();
		}

		// Return a referece to this container allocator instance
		Allocator& GetAllocator() const
		{
			return allocator;
		}

		// Compacts the tree nodes making them consecutive
		// this will reallocate the hole tree
		void Compact()
		{
			//TODO:
		}

		// Return the size of the tree
		_INLINE uint32 Size()
		{
			return _size;
		}

		// Clear the tree by removing all its nodes
		_INLINE void Clear()
		{
			_Clear(_root);

			_root = _nil;
		}

		// Return the minimun node in the tree
		NodePointer Min(NodePointer node)
		{
			while (node->_left != _nil)
				node = node->_left;

			return node;
		}

		// Return the miximun value in the tree
		NodePointer Max(NodePointer node)
		{
			while (node->_right != _nil)
				node = node->_right;

			return node;
		}

		// 
		_INLINE const NodePointer Nil() const
		{
			return (const NodePointer)_nil;
		}

		_INLINE bool IsNil(NodePointer node) const 
		{
			return node == _nil;
		}

		// Return a reference to the root node
		_INLINE NodePointer& Root()
		{
			return _root;
		}

		// Return true if the tree is empty
		_INLINE bool Empty()
		{
			return _size == 0;
		}

		// Return the given node successor
		NodePointer Successor(NodePointer x)
		{

			/*if (x->_right != _nil)
				return Min(x->_right);

			return _nil;*/

			if (x->_right != _nil)
				return Min(x->_right);

			NodePointer y = x->_parent;

			while (y != _nil && x == y->_right)
			{
				x = y;
				y = y->_parent;
			}

			return y;
		}

		// Return the given node successor
		NodePointer Predecessor(NodePointer x)
		{
			if (x->_left != _nil)
				return Max(x->_left);

			NodePointer y = x->_parent;

			while (y != _nil && x == y->_left)
			{
				x = y;
				y = y->_parent;
			}

			return y;
		}

		// Return the traits object related to this tree
		_INLINE _Traits Traits() const
		{
			return _traits;
		}

		// Create an iterator to a node
		// 
		// @param: The start node
		// @return: The iterator object
		iterator CreateIterator(NodePointer node)
		{
			return iterator(node, this);
		}

		// Create an iterator to the begining of the tree
		iterator Begin()
		{
			return iterator(Min(_root), this);
		}

		// Create an iterator to the end of the tree
		iterator End()
		{
			return iterator(_nil, this);
		}

		// Create a reverese iterator to a node
		// 
		// @param: The start node
		// @return: The iterator object
		reverse_iterator CreateRevereseIterator(NodePointer node)
		{
			return reverse_iterator(node, this);
		}

		// Create a reverese iterator to the begining of the tree
		reverse_iterator rBegin()
		{
			return reverse_iterator(Max(_root), this);
		}

		// Create a reverese iterator to the end of the tree
		reverse_iterator rEnd()
		{
			return reverse_iterator(_nil, this);
		}

		Node* NodeOf(NodePointer node)
		{
			return node.Get();
		}

	protected:

		void _Init()
		{
			allocator.Init();

			_nil = allocator.Allocate();
			//_nil->_isNil = _TRUE;
			_nil->_color = TREE_COLOR_BLACK;
			_nil->_left = Allocator::null_handle();
			_nil->_right = Allocator::null_handle();

			_root = _nil;
			_root->_left = _nil;
			_root->_right = _nil;
			_root->_parent = _nil;
		}

		// Insert a node to the tree using normal binary search tree insertion
		void _Insert(NodePointer z, NodePointer hint)
		{
			NodePointer y = _nil;
			NodePointer x = hint;

			while (x != _nil)
			{
				y = x;

				if (_traits.Compare(_traits.KeyOf(z->_value), _traits.KeyOf(x->_value)) < 0)
					x = x->_left; // Go left
				else
					x = x->_right; // >= Go right
			}

			// Set new node parent
			z->_parent = y;

			// Set new node left or right 
			if (y == _nil)
				_root = z;
			else if (_traits.Compare(_traits.KeyOf(z->_value), _traits.KeyOf(y->_value)) < 0)
				y->_left = z;
			else
				y->_right = z;

			// Init new node color and children
			z->_left = _nil;
			z->_right = _nil;
			z->_color = TREE_COLOR_RED;

			++_size;

			// Check & fix tree properties
			_InsertFix(z);
		}

		// Fixes an insertion by apply rb tree rules to a given node
		void _InsertFix(NodePointer z)
		{
			NodePointer y = _nil;

			while (z->_parent->_color == TREE_COLOR_RED)
			{

				if (z->_parent == z->_parent->_parent->_left)
				{
					y = z->_parent->_parent->_right;

					// Case 1: If y is color, then recolor nodes
					if (y->_color == TREE_COLOR_RED)
					{
						z->_parent->_color = TREE_COLOR_BLACK;
						y->_color = TREE_COLOR_BLACK;
						z->_parent->_parent->_color = TREE_COLOR_RED;
						z = z->_parent->_parent;
					}

					// Case 2: If y is black and z is a right child
					else if (z == z->_parent->_right)
					{
						// left rotate around z
						z = z->_parent;
						RotateLeft(z);
					}

					// Case 3: y is black and z is a left child
					else
					{
						// Recolor and grandpa right rotation
						z->_parent->_color = TREE_COLOR_BLACK;
						z->_parent->_parent->_color = TREE_COLOR_RED;
						RotateRight(z->_parent->_parent);
					}
				}
				else
				{
					y = z->_parent->_parent->_left;

					if (y->_color == TREE_COLOR_RED)
					{
						z->_parent->_color = TREE_COLOR_BLACK;
						y->_color = TREE_COLOR_BLACK;
						z->_parent->_parent->_color = TREE_COLOR_RED;
						z = z->_parent->_parent;
					}
					else if (z == z->_parent->_left)
					{
						z = z->_parent;
						RotateRight(z);
					}
					else
					{
						z->_parent->_color = TREE_COLOR_BLACK;
						z->_parent->_parent->_color = TREE_COLOR_RED;
						RotateLeft(z->_parent->_parent);
					}
				}
			}

			// Root always black
			_root->_color = TREE_COLOR_BLACK;
		}

		// fixup remove rules
		void _RemoveFix(NodePointer x)
		{
			NodePointer w = Allocator::null_handle();

			while (x != _root && x->_color == TREE_COLOR_BLACK)
			{
				if (x == x->_parent->_left)
				{
					w = x->_parent->_right;

					if (w->_color == TREE_COLOR_RED) // w is ref
					{
						w->_color = TREE_COLOR_BLACK;
						x->_parent->_color = TREE_COLOR_RED;
						RotateLeft(x->_parent);
						w = x->_parent->_right;
					}

					if (w->_left->_color == TREE_COLOR_BLACK // w left is red
						&& w->_right->_color == TREE_COLOR_BLACK)
					{
						w->_color = TREE_COLOR_RED;
						x = x->_parent;
					}
					else
					{
						if (w->_right->_color == TREE_COLOR_BLACK) // w right is black
						{
							w->_left->_color = TREE_COLOR_BLACK;
							w->_color = TREE_COLOR_RED;
							RotateRight(w);
							w = x->_parent->_right;
						}

						w->_color = x->_parent->_color;
						x->_parent->_color = TREE_COLOR_BLACK;
						w->_right->_color = TREE_COLOR_BLACK;
						RotateLeft(x->_parent);
						x = _root;
					}
				}
				else
				{
					w = x->_parent->_left;

					if (w->_color == TREE_COLOR_RED)
					{
						w->_color = TREE_COLOR_BLACK;
						x->_parent->_color = TREE_COLOR_RED;
						RotateRight(x->_parent);
						w = x->_parent->_left;
					}

					if (w->_right->_color == TREE_COLOR_BLACK 
						&& w->_left->_color == TREE_COLOR_BLACK)
					{
						w->_color = TREE_COLOR_RED;
						x = x->_parent;
					}
					else
					{
						if (w->_left->_color == TREE_COLOR_BLACK)
						{
							w->_right->_color = TREE_COLOR_BLACK;
							w->_color = TREE_COLOR_RED;
							RotateLeft(w);
							w = x->_parent->_left;
						}

						w->_color = x->_parent->_color;
						x->_parent->_color = TREE_COLOR_BLACK;
						w->_left->_color = TREE_COLOR_BLACK;
						RotateRight(x->_parent);
						x = _root;
					}
				}
			}

			x->_color = TREE_COLOR_BLACK;
		}

		// Search for a key in this tree
		//
		// @param: key - the key to search for
		// @return: Pointer to found key's node or nullptr
		NodePointer _Find(const KeyType& key)
		{
			NodePointer current = _root;

			while (current != _nil)
			{
				auto comp = _traits.Compare(key, _traits.KeyOf(current->_value));

				if (comp == 0)
					return current;
				else if (comp > 0)
					current = current->_right;
				else if (comp < 0)
					current = current->_left;
			}

			// Nothing found
			return Allocator::null_handle();
		}

		// Apply a left rotation to a node
		void RotateLeft(NodePointer x)
		{
			// Perfrom the rotation
			NodePointer y = x->_right;
			x->_right = y->_left;

			// Update and fix pointers
			if (y->_left != _nil)
				y->_left->_parent = x;
			y->_parent = x->_parent;

			// Update root if requires
			if (x->_parent == _nil)
				_root = y;
			else if (x->_parent->_left == x)
				x->_parent->_left = y;
			else
				x->_parent->_right = y;

			y->_left = x;
			x->_parent = y;
		}

		// Apply a left rotation to a node
		void RotateRight(NodePointer y)
		{
			NodePointer x = y->_left;
			y->_left = x->_right;

			if (x->_right != _nil)
				x->_right->_parent = y;
			x->_parent = y->_parent;

			if (y->_parent == _nil)
				_root = x;
			else if (y->_parent->_right == y)
				y->_parent->_right = x;
			else
				y->_parent->_left = x;

			x->_right = y;
			y->_parent = x;
		}

		// Clear by remove all given node and its children
		void _Clear(NodePointer node)
		{
			if (!node.IsNull() && node != _nil)
			{
				_Clear(node->_right);
				_Clear(node->_left);

				(node->_value).~ValueType();
				allocator.Deallocate(node.GetHandle());
				--_size;
			}
		}

	private:

		// This is the tree's root node,
		// We always start here
		NodePointer _root;

		// Null node for leafs and null uses
		NodePointer _nil;

		// The traits object,
		// that allow tree to behaive like other containers types
		_Traits _traits;

		// Count of elements in the tree
		uint32 _size;

		// Define allocator type
		Allocator allocator;

	};

	// TODO: To Add
	// 1- Compact
	// 2- Recycling nodes
}
