#pragma once

#include "BTreePrimitives.hpp"
#include "Utils/Optional.hpp"

namespace Poly
{
	/**
	 * <summary>
	 * A sorted map based on a B-Tree.
	 * </summary>
	 *
	 * <typeparam name="K">Key type</typeparam>
	 * <typeparam name="V">Value type</typeparam>
	 * <typeparam name='Bfactor'>
	 * Each node contains B-1 to 2*B-1 elements.
	 * Increasing B reduces allocations and improves cache locality in searches, but hurts complexity O(B*log_B(n)).
	 * Default value is 6.
	 * </typeparam>
	 *
	 * If you do not need the elements to be ordered <see cref="HashMap<K, V>"/>, which is generally faster
	 */
	template<typename K, typename V, size_t Bfactor = 6>
	class OrderedMap final : public BaseObjectLiteralType<>
	{
		static_assert(Bfactor > 1, "B factor must be greater than 1. Consider using a classic binary tree if you need a lesser value.");
		using BTree    = Impl::BTree<K, V, Bfactor>;
		using Root     = typename BTree::Root;
		using LeafNode = typename BTree::LeafNode;
		using KVERef   = typename BTree::KVERef;
		using NodeRef  = typename BTree::NodeRef;
		template<typename> class MapEntry; //note(vuko): can't be named Entry due to symbol collision (class/method -> ok, template/method -> nope ;_;)
		class ConstIterator;
		class Iterator;
		class Keys; //note(vuko): key references are always const; allowing to modify them would break the order invariant
		class ConstValues;
		class Values;
	public:
		static constexpr size_t B = Bfactor;

		/// <summary>Constructs a new, empty <c>OrderedMap<K, V, B></c>. The map will not allocate until elements are inserted into it. </summary>
		OrderedMap() : root{nullptr, 0}, len(0) {}
		OrderedMap(OrderedMap&& other) : root(other.root), len(other.len) { ObjectLifetimeHelper::DefaultCreate(&other); }
		OrderedMap(const OrderedMap& other) : OrderedMap() { for (auto kv : other) { Insert(kv.key, kv.value); } } //todo(vuko): can be implemented more efficiently
		~OrderedMap() { if (root.node) { Clear(); delete root.node; } };

		OrderedMap& operator=(OrderedMap&& other)
		{
			ObjectLifetimeHelper::Destroy(this);
			this->root = other.root;
			this->len  = other.len;
			ObjectLifetimeHelper::DefaultCreate(&other);
			return *this;
		}
		OrderedMap& operator=(const OrderedMap& other) { ObjectLifetimeHelper::Destroy(this); ObjectLifetimeHelper::CopyCreate(this, other); return *this; }

		/**
		 * <summary>
		 * Gets the given key's corresponding entry in the map for in-place manipulation.
		 * Examples:
		 * <code>
		 * if (auto entry = map.Entry(key)) //`if (auto entry = map.Entry(key); !entry.IsVacant())` in C++17
		 * {
		 * 	entry.Remove().DoStuffWithOldValue();
		 * 	//or
		 * 	entry.OccupiedReplace(newValue);
		 * 	//or
		 * 	entry.OccupiedGet().DoStuffWithValue();
		 * }
		 * else
		 * {
		 * 	entry.VacantInsert(value);
		 * }
		 *
		 * map.Entry(key).OrInsert(Dynarray<int>()).PushBack(8);
		 * </code>
		 * </summary>
		 * <param name="key"></param>
		 * <returns>A view into a single entry in a map, which may either be vacant or occupied.</returns>
		 * <seealso cref="MapEntry"/>
		 */
		MapEntry<const K& > Entry(const K&  key) { return EntryPimple(key); }
		MapEntry<      K&&> Entry(      K&& key) { return EntryPimple(std::move(key)); }

		/**
		 * <summary>Inserts a key-value pair into the map. Updates the value if the key was already present.</summary>
		 * <param name="key"></param>
		 * <param name="value"></param>
		 * <returns>The old value if it was present.</returns>
		 * <remarks>The key is not updated. This may matter for types that can be equal without being identical.</remarks>
		 */
		Optional<V> Insert(const K&  key, const V&  value) { return InsertPimple(          key ,           value ); }
		Optional<V> Insert(const K&  key,       V&& value) { return InsertPimple(          key , std::move(value)); }
		Optional<V> Insert(      K&& key, const V&  value) { return InsertPimple(std::move(key),           value ); }
		Optional<V> Insert(      K&& key,       V&& value) { return InsertPimple(std::move(key), std::move(value)); }

		/**
		 * <summary>Inserts a key-value pair into the map. Panics if the key was already present.</summary>
		 * <param name="key"></param>
		 * <param name="value"></param>
		 */
		void MustInsert(const K&  key, const V&  value) { auto entry = Entry(          key ); ASSERTE(entry.IsVacant(), "Key already present in the map!"); entry.VacantInsert(          value) ; }
		void MustInsert(const K&  key,       V&& value) { auto entry = Entry(          key) ; ASSERTE(entry.IsVacant(), "Key already present in the map!"); entry.VacantInsert(std::move(value)); }
		void MustInsert(      K&& key, const V&  value) { auto entry = Entry(std::move(key)); ASSERTE(entry.IsVacant(), "Key already present in the map!"); entry.VacantInsert(          value) ; }
		void MustInsert(      K&& key,       V&& value) { auto entry = Entry(std::move(key)); ASSERTE(entry.IsVacant(), "Key already present in the map!"); entry.VacantInsert(std::move(value)); }

		/**
		 * <summary>Removes a key from the map.</summary>
		 * <param name="key"></param>
		 * <returns>Value at key if it was present in the map.</returns>
		 */
		Optional<V> Remove(const K&  key) { return RemovePimple(key); }
		Optional<V> Remove(      K&& key) { return RemovePimple(std::move(key)); }

		/**
		 * <summary>Removes a key from the map. Panics if the key is not present in the map.</summary>
		 * <param name="key"></param>
		 * <returns>Value at key.</returns>
		 */
		V MustRemove(const K&  key) { auto entry = Entry(          key ); ASSERTE(!entry.IsVacant(), "Key not present in the map!"); return entry.Remove(); }
		V MustRemove(      K&& key) { auto entry = Entry(std::move(key)); ASSERTE(!entry.IsVacant(), "Key not present in the map!"); return entry.Remove(); }

		/**
		 * <summary>Get a reference to the value at key.</summary>
		 * <param name="key"></param>
		 * <returns>The reference if the keys is in the map.</returns>
		 */
		Optional<V&> Get(const K&  key) { return GetPimple(key); }
		Optional<V&> Get(      K&& key) { return GetPimple(std::move(key)); }
		Optional<const V&> Get(const K&  key) const { return GetPimple(key); }
		Optional<const V&> Get(      K&& key) const { return GetPimple(std::move(key)); }

		      V& operator[](const K& key)       { return Get(key).Value(); }
		const V& operator[](const K& key) const { return Get(key).Value(); }

		/// <returns>The number of elements in the map.</returns>
		size_t GetSize() const { return len; };
		/// <returns>True if the map contains no elements.</returns>
		bool IsEmpty() const { return GetSize() == 0; };

		/// <summary>Clears the map, removing all elements. Frees all memory except the root node.</summary>
		void Clear()
		{
			if (root.node == nullptr)
			{
				return;
			}

			KVERef current = BTree::FirstLeafEdge(root.AsNodeRef());
			while (len)
			{
				len -= 1;
				if (current.idx < current.nodeRef.node->len)
				{
					current = KVERef{current.nodeRef, current.idx + 1};
					continue;
				}
				else
				{
					const NodeRef toDelete = current.nodeRef;
					current = current.nodeRef.Ascend().parent;
					delete toDelete.node;
				}

				for (;;)
				{
					if (current.idx < current.nodeRef.node->len)
					{
						current = BTree::FirstLeafEdge(KVERef{current.nodeRef, current.idx + 1}.Descend());
						break; //continue outer loop
					}
					else
					{
						const NodeRef toDelete = current.nodeRef;
						current = current.nodeRef.Ascend().parent;
						delete toDelete.node->AsBranch();
					}
				}
			}

			LeafNode* newRootToBe = current.nodeRef.node;

			auto ascension = current.nodeRef.Ascend();
			while (ascension.succeeded)
			{
				KVERef node = ascension.parent;
				ascension = node.nodeRef.Ascend();
				delete node.nodeRef.node->AsBranch();
			}

			//reinitialize the root node
			ObjectLifetimeHelper::Destroy(newRootToBe);
			ObjectLifetimeHelper::DefaultCreate(newRootToBe);

			root = {newRootToBe, 0};
		}

		ConstIterator cbegin() const { return {BTree::FirstLeafEdge(const_cast<OrderedMap&>(*this).root.AsNodeRef()), 0, GetSize()}; }
		ConstIterator cend() const
		{
			if (root.node == nullptr)
			{
				return cbegin();
			}
			return {BTree::LastLeafEdge(const_cast<OrderedMap&>(*this).root.AsNodeRef()), GetSize(), GetSize()};
		}

		ConstIterator begin() const { return cbegin(); }
		ConstIterator end()   const { return cend();   }

		Iterator begin() { return {BTree::FirstLeafEdge(root.AsNodeRef()), 0, GetSize()}; }
		Iterator end()
		{
			if (root.node == nullptr)
			{
				return begin();
			}
			return {BTree::LastLeafEdge(root.AsNodeRef()), GetSize(), GetSize()};
		}

		/// <returns>A range over the keys of the map, in sorted order.</returns>
		class Keys   Keys()   const { return {*this}; }
		/// <returns>A range over the values of the map, in order by key.</returns>
		class Values Values()       { return {*this}; }
		ConstValues  Values() const { return {*this}; }

		/// <summary>Swaps the contents of this map with the other map.</summary>
		void Swap(OrderedMap& other) { std::swap(this->root, other.root); std::swap(this->len, other.len); }

	private:
		template<typename Key>
		MapEntry<Key&&> EntryPimple(Key&& key)
		{
			const auto searchResult = SearchTree(root.AsNodeRef(), key);
			switch (searchResult.result)
			{
				case SearchResult::FOUND:  return MapEntry<Key&&>::Occupied(searchResult.handle, len);
				case SearchResult::FAILED: return MapEntry<Key&&>::Vacant(  searchResult.handle, len, std::forward<Key>(key));
				default: UNREACHABLE();
			}
		}

		template<typename Key, typename Val>
		Optional<V> InsertPimple(Key&& key, Val&& value)
		{
			auto entry = Entry(std::forward<Key>(key));
			if (entry.IsVacant())
			{
				entry.VacantInsert(std::forward<Val>(value));
				return {};
			}
			else
			{
				return {entry.OccupiedReplace(std::forward<Val>(value))};
			}
		}

		template<typename Key>
		Optional<V> RemovePimple(Key&& key)
		{
			auto entry = Entry(std::forward<Key>(key));
			if (!entry.IsVacant())
			{
				return {entry.Remove()};
			}
			else
			{
				return {};
			}
		}

		template<typename Key>
		Optional<const V&> GetPimple(Key&& key) const
		{
			auto entry = const_cast<OrderedMap&>(*this).Entry(std::forward<Key>(key)); //note(vuko): using only const pathways down the line
			if (!entry.IsVacant())
			{
				return {entry.OccupiedGet()};
			}
			else
			{
				return {};
			}
		}

		template<typename Key>
		Optional<V&> GetPimple(Key&& key)
		{
			auto entry = Entry(std::forward<Key>(key));
			if (!entry.IsVacant())
			{
				return {entry.OccupiedGet()};
			}
			else
			{
				return {};
			}
		}

		/// <see cref="OrderedMap::Entry()"/>
		template<typename Key>
		class MapEntry final : public BaseObjectLiteralType<>
		{
		public:
			static MapEntry Vacant  (KVERef kvRef, size_t& len, Key&& key) { return MapEntry(kvRef, len, std::forward<Key>(key)); }
			static MapEntry Occupied(KVERef kvRef, size_t& len)            { return MapEntry(kvRef, len); }

			MapEntry(MapEntry&&) = default;

			/// <returns>True if the entry is vacant.</returns>
			bool IsVacant() const { return static_cast<bool>(key); }
			operator bool() const { return !IsVacant(); }

			/**
			 * <summary>Ensures a value is in the map by inserting if vacant.</summary>
			 * <param name="value">A value to insert if the entry is vacant.</param>
			 * <returns>Reference to the value in the map.</returns>
			 */
			V& OrInsert(const V&  value) { return OrInsertPimple(value); }
			V& OrInsert(      V&& value) { return OrInsertPimple(std::move(value)); }

			/**
			 * <summary>Inserts the value into the map at entry's key.</summary>
			 * <param name="value"></param>
			 * <returns>Reference to the value in the map.</returns>
			 */
			V& VacantInsert(const V&  value) { return VacantInsertPimple(value); }
			V& VacantInsert(      V&& value) { return VacantInsertPimple(std::move(value)); }

			/**
			 * <summary>Replaces the value in the map at entry's key.</summary>
			 * <param name="value"></param>
			 * <returns>Old value.</returns>
			 */
			V OccupiedReplace(const V&  value) { return OccupiedReplacePimple(value); }
			V OccupiedReplace(      V&& value) { return OccupiedReplacePimple(std::move(value)); }

			/// <returns>A reference to the value in the entry.</returns>
			const V& OccupiedGet() const { ASSERTE(!IsVacant(), "Cannot read value from a vacant map entry!"); return kvRef.nodeRef.node->values[kvRef.idx]; }
			V& OccupiedGet() { return const_cast<V&>(const_cast<const MapEntry&>(*this).OccupiedGet()); }

			/// <summary>A key-value pair.</summary>
			struct KV final
			{
				K key;
				V value;
			};
			/**
			 * <summary>Removes the entry from the map.</summary>
			 * <returns>Removed key-value pair.</returns>
			 */
			KV RemoveKV()
			{
				ASSERTE(!IsVacant(), "Cannot remove value from a vacant map entry!");

				mapLen -= 1;

				auto handleUnderflow = [](NodeRef curNode)
				{
					while (curNode.node->len < BTree::MIN_LEN)
					{
						const auto ascension = curNode.Ascend();
						if (!ascension.succeeded)
						{
							break; //root is allowed to underflow
						}

						const KVERef parent = ascension.parent;
						bool isLeft;
						KVERef handle;
						if (parent.idx > 0)
						{
							isLeft = true;
							handle = {parent.nodeRef, parent.idx - 1};
						}
						else
						{
							isLeft = false;
							handle = {parent.nodeRef, parent.idx};
						}

						if (!handle.CanMerge())
						{
							if (isLeft)
							{
								handle.StealLeft();
							}
							else
							{
								handle.StealRight();
							}
							break;
						}

						NodeRef nodeRef = handle.Merge().nodeRef;
						if (nodeRef.node->len == 0)
						{
							//a node of zero length; only root is allowed to underflow, therefore we must be at root
							//the root is empty with only one child leaf, get rid of it
							nodeRef.root->PopLevel();
							break;
						}

						curNode = nodeRef;
					}
				};

				if(kvRef.nodeRef.height == 0)
				{
					//leaf
					const auto removeResult = kvRef.LeafRemove();
					handleUnderflow(removeResult.nodeRef);
					return KV{std::move(removeResult.k), std::move(removeResult.v)};
				}
				else
				{
					//branch
					auto& keyRef = kvRef.nodeRef.node->keys  [kvRef.idx];
					auto& valRef = kvRef.nodeRef.node->values[kvRef.idx];

					auto toRemove = BTree::FirstLeafEdge(KVERef{kvRef.nodeRef, kvRef.idx + 1}.Descend());
					auto removeResult = toRemove.LeafRemove();

					std::swap(keyRef, removeResult.k);
					std::swap(valRef, removeResult.v);

					handleUnderflow(removeResult.nodeRef);
					return KV{std::move(removeResult.k), std::move(removeResult.v)};
				}
			}

			/**
			 * <summary>Removes the entry from the map.</summary>
			 * <returns>Removed value.</returns>
			 * <seealso cref="RemoveKV"/>
			 */
			V Remove() { return RemoveKV().value; }

		private:
			template<typename Val>
			V& OrInsertPimple(Val&& value)
			{
				if (key)
				{
					return VacantInsert(std::forward<Val>(value));
				}
				else
				{
					return OccupiedGet();
				}
			}

			template<typename Val>
			V& VacantInsertPimple(Val&& value)
			{
				ASSERTE(IsVacant(), "Cannot insert into already occupied map entry!");

				if (kvRef.nodeRef.node == nullptr)
				{
					//we've got nothing to operate on; plant the little happy tree first!
					auto sapling = new LeafNode();
					kvRef.nodeRef.node       = sapling;
					kvRef.nodeRef.root->node = sapling;
				}

				mapLen += 1;

				auto insertResult = kvRef.LeafInsert(key.TakeValue(), std::forward<Val>(value));
				if (insertResult.fit)
				{
					return *insertResult.valueRef;
				}
				//insertion resulted in a node split; need to re-insert the extracted key-value pair

				V* v = insertResult.valueRef; //points to the value we just inserted
				auto ascensionResult = insertResult.splitResult.oldNodeLeft.Ascend();

				for (;;)
				{
					if (ascensionResult.succeeded)
					{
						insertResult = ascensionResult.parent.BranchInsert(std::move(insertResult.splitResult.key), std::move(insertResult.splitResult.value), insertResult.splitResult.newEdgeRight);
						if (insertResult.fit)
						{
							return *v;
						}
						ascensionResult = insertResult.splitResult.oldNodeLeft.Ascend();
					}
					else
					{
						Root* root = ascensionResult.self.root;
						root->PushLevel().PushBack(std::move(insertResult.splitResult.key), std::move(insertResult.splitResult.value), insertResult.splitResult.newEdgeRight);
						return *v;
					}
				}
			}

			template<typename Val>
			V OccupiedReplacePimple(Val&& value)
			{
				ASSERTE(!IsVacant(), "Cannot replace (non-existent) value in a vacant map entry!");
				auto old = std::move(OccupiedGet());
				OccupiedGet() = std::forward<Val>(value);
				return old;
			}

			MapEntry(KVERef kvRef, size_t& len, Key&& key) : key(std::forward<Key>(key)), kvRef(kvRef), mapLen(len) {}
			MapEntry(KVERef kvRef, size_t& len)            : key(),                       kvRef(kvRef), mapLen(len) {}

			Optional<Key> key;
			KVERef  kvRef;
			size_t& mapLen;
		};

		struct ConstKV final
		{
			const K& key;
			const V& value;
			const ConstKV* operator->() const { return this; } //note(vuko): needed by the iterator's own operator ->, since it returns KV by value
		};
		struct KV final
		{
			const K& key;
			V& value;
			KV* operator->() { return this; }
		};

		template<typename RetKV>
		class IteratorBase : public std::iterator<std::bidirectional_iterator_tag, RetKV>, public BaseObjectLiteralType<>
		{
		public:
			IteratorBase(KVERef kvRef, size_t position, size_t mapLen) : current(kvRef), nextForward(kvRef), nextBackward(kvRef), position(position), mapLen(mapLen)
			{
				if (position != mapLen)
				{
					if (kvRef.idx < kvRef.nodeRef.node->len)
					{
						nextForward = KVERef{kvRef.nodeRef, kvRef.idx + 1};
					}
				}
			}

			bool operator==(const IteratorBase& other) const { return this->current == other.current; }
			bool operator!=(const IteratorBase& other) const { return !(*this == other); }

			IteratorBase& operator++() { nextBackward = current; current = Next(); return *this; }
			IteratorBase& operator--() { nextForward  = current; current = Prev(); return *this; }
			IteratorBase operator++(int) { IteratorBase ret(current, position, mapLen); operator++(); return ret; }
			IteratorBase operator--(int) { IteratorBase ret(current, position, mapLen); operator--(); return ret; }
		private:
			KVERef Next()
			{
				KVERef kvRef = nextForward;

				if (position + 1u >= mapLen)
				{
					return kvRef;
				}
				position += 1u;

				if (kvRef.idx < kvRef.nodeRef.node->len)
				{
					nextForward = KVERef{kvRef.nodeRef, kvRef.idx + 1u};
					return kvRef;
				}

				do
				{
					kvRef = kvRef.nodeRef.Ascend().parent;
				}
				while (kvRef.idx >= kvRef.nodeRef.node->len);

				nextForward = BTree::FirstLeafEdge(KVERef{kvRef.nodeRef, kvRef.idx + 1u}.Descend());
				return kvRef;
			}
			KVERef Prev()
			{
				KVERef kvRef = nextBackward;

				if (position <= 1u)
				{
					nextForward = KVERef{current.nodeRef, current.idx + 1 - position};
					position = 0;
					return kvRef;
				}
				position -= 1u;

				if (kvRef.idx > 0)
				{
					nextBackward = KVERef{kvRef.nodeRef, kvRef.idx - 1u};
					return kvRef;
				}

				do
				{
					kvRef = kvRef.nodeRef.Ascend().parent;
				}
				while (kvRef.idx == 0);

				nextBackward = BTree::LastLeafEdge(KVERef{kvRef.nodeRef, kvRef.idx - 1u}.Descend());
				return kvRef;
			}
		protected:
			KVERef current;
			KVERef nextForward;
			KVERef nextBackward;
			size_t position;
			size_t mapLen;
		};

		class ConstIterator final : public IteratorBase<ConstKV>
		{
			using IteratorBase<ConstKV>::current;
		public:
			using IteratorBase<ConstKV>::IteratorBase;
			ConstKV operator*()  const { return {current.nodeRef.node->keys[current.idx], current.nodeRef.node->values[current.idx]}; }
			ConstKV operator->() const { return operator*(); }
		};

		class Iterator final : public IteratorBase<KV>
		{
			using IteratorBase<KV>::current;
		public:
			using IteratorBase<KV>::IteratorBase;
			KV operator*()  { return {current.nodeRef.node->keys[current.idx], current.nodeRef.node->values[current.idx]}; }
			KV operator->() { return operator*(); }
		};

		/// <see cref="OrderedMap::Keys()"/>
		class Keys final : public BaseObjectLiteralType<>
		{
			class Iter;
		public:
			Keys(const OrderedMap& map) : map(map) {}

			Iter cbegin() const { return {map.cbegin()}; }
			Iter cend()   const { return {map.cend()  }; }
			Iter begin()  const { return cbegin(); }
			Iter end()    const { return cend(); }
		private:
			using ParentIter = typename OrderedMap::ConstIterator;
			class Iter final : public std::iterator<std::bidirectional_iterator_tag, K>
			{
			public:
				Iter(ParentIter iter) : iter(iter) {}

				bool operator==(const Iter& other) const { return this->iter == other.iter; }
				bool operator!=(const Iter& other) const { return !(*this == other); }

				const K& operator*()  { return iter->key; }
				const K& operator->() { return operator*(); }

				Iter& operator++() { ++(iter); return *this; }
				Iter& operator--() { --(iter); return *this; }
				Iter operator++(int) { Iter ret(iter); operator++(); return ret; }
				Iter operator--(int) { Iter ret(iter); operator--(); return ret; }
			private:
				ParentIter iter;
			};

			const OrderedMap& map;
		};

		/// <see cref="OrderedMap::Values()"/>
		class ConstValues final : public BaseObjectLiteralType<>
		{
			class Iter;
		public:
			ConstValues(const OrderedMap& map) : map(map) {}

			Iter cbegin() const { return {map.cbegin()}; }
			Iter cend()   const { return {map.cend()}; }
			Iter begin()  const { return cbegin(); }
			Iter end()    const { return cend(); }
		private:
			using ParentIter = typename OrderedMap::ConstIterator;
			class Iter final : public std::iterator<std::bidirectional_iterator_tag, V>
			{
			public:
				Iter(ParentIter iter) : iter(iter) {}

				bool operator==(const Iter& other) const { return this->iter == other.iter; }
				bool operator!=(const Iter& other) const { return !(*this == other); }

				Iter& operator++() { ++(iter); return *this; }
				Iter& operator--() { --(iter); return *this; }
				Iter operator++(int) { Iter ret(iter); operator++(); return ret; }
				Iter operator--(int) { Iter ret(iter); operator--(); return ret; }

				const V& operator*()  { return iter->value; }
				const V& operator->() { return operator*(); }
			private:
				ParentIter iter;
			};

			const OrderedMap& map;
		};

		/// <see cref="OrderedMap::Values()"/>
		class Values final : public BaseObjectLiteralType<>
		{
			class Iter;
		public:
			Values(OrderedMap& map) : map(map) {}

			Iter begin() { return {map.begin()}; }
			Iter end()   { return {map.end()}; }
		private:
			using ParentIter = typename OrderedMap::Iterator;
			class Iter final : public std::iterator<std::bidirectional_iterator_tag, V>
			{
			public:
				Iter(ParentIter iter) : iter(iter) {}

				bool operator==(const Iter& other) const { return this->iter == other.iter; }
				bool operator!=(const Iter& other) const { return !(*this == other); }

				Iter& operator++() { ++(iter); return *this; }
				Iter& operator--() { --(iter); return *this; }
				Iter operator++(int) { Iter ret(iter); operator++(); return ret; }
				Iter operator--(int) { Iter ret(iter); operator--(); return ret; }

				V& operator*()  { return iter->value; }
				V& operator->() { return operator*(); }
			private:
				ParentIter iter;
			};

			OrderedMap& map;
		};

		struct SearchResult final
		{
			enum { FOUND, DESCEND, FAILED = DESCEND } result;
			KVERef handle;
		};

		static SearchResult SearchTree(NodeRef nodeRef, const K& key)
		{
			if (nodeRef.node == nullptr)
			{
				return {SearchResult::FAILED, KVERef{nodeRef, 0}};
			}

			for (;;)
			{
				const auto searchResult = SearchNode(nodeRef, key);
				switch (searchResult.result)
				{
					case SearchResult::FOUND: return {SearchResult::FOUND, searchResult.handle};
					case SearchResult::DESCEND:
					{
						const auto handle = searchResult.handle;
						if (handle.nodeRef.height == 0)
						{
							//leaf
							return {SearchResult::FAILED, handle};
						}
						//branch
						nodeRef = handle.Descend();
					}
				}
			}
		}

		static SearchResult SearchNode(const NodeRef nodeRef, const K& key)
		{
			return SearchLinear(nodeRef, key);
			//todo(vuko): possibly switch to binary search when B is large enough (how large?)
		}

		template<typename T> static auto EqualityCheck(const T& a, const T& b, int) -> decltype(a == b) { return a == b; } //note(vuko): templating and decltype are used for SFINAE here
		template<typename T> static auto EqualityCheck(const T& a, const T& b, ...) -> decltype(bool{}) { return !(a < b || b < a); }
		static bool Equals(const K& a, const K& b) { constexpr int choose{}; return EqualityCheck(a, b, choose); }

		static SearchResult SearchLinear(const NodeRef nodeRef, const K& searchKey)
		{
			const LeafNode* const node = nodeRef.node;
			for (size_t i = 0; i < node->len; i++)
			{
				const K& testedKey = node->keys[i];
				if (searchKey < testedKey)
				{
					return {SearchResult::DESCEND, KVERef{nodeRef, i}};
				}
				else if (Equals(searchKey, testedKey))
				{
					return {SearchResult::FOUND, KVERef{nodeRef, i}};
				}
			}
			return {SearchResult::DESCEND, KVERef{nodeRef, node->len}};
		}

		static SearchResult SearchBinary(NodeRef nodeRef, const K& searchKey);

		Root root;
		size_t len;
	};

} //namespace Poly

template<typename K, typename V, size_t B> void swap(Poly::OrderedMap<K, V, B>& lhs, Poly::OrderedMap<K, V, B>& rhs) { lhs.Swap(rhs); };
