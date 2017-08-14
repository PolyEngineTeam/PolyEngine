#pragma once

#include "BTreeNode.hpp"
#include "Optional.hpp"

namespace Poly {
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
	class OrderedMap {
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
		OrderedMap(OrderedMap&& other) : root(other.root), len(other.len) { ::new(&other) OrderedMap(); }
		OrderedMap(const OrderedMap& other) : OrderedMap() { for (auto kv : other) { Insert(kv.key, kv.value); } } //todo(vuko): can be implemented more efficiently
		~OrderedMap() { if (root.node) { Clear(); delete root.node; } };

		OrderedMap& operator=(OrderedMap&& other) {
			this->~OrderedMap();
			this->root = other.root;
			this->len  = other.len;
			::new(&other) OrderedMap();
			return *this;
		}
		OrderedMap& operator=(const OrderedMap& other) { this->~OrderedMap(); ::new(this) OrderedMap(other); return *this; }

		/**
		 * <summary>
		 * Gets the given key's corresponding entry in the map for in-place manipulation.
		 * Examples:
		 * <code>
		 * if (auto entry = map.Entry(key)) { //`if (auto entry = map.Entry(key); !entry.IsVacant())` in C++17
		 * 	entry.Remove().DoStuffWithOldValue();
		 * 	//or
		 * 	entry.OccupiedReplace(newValue);
		 * 	//or
		 * 	entry.OccupiedGet().DoStuffWithValue();
		 * } else {
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
		MapEntry<const K& > Entry(const K&  key) { return entry(key); }
		MapEntry<      K&&> Entry(      K&& key) { return entry(std::move(key)); }

		/**
		 * <summary>Inserts a key-value pair into the map. Updates the value if the key was already present.</summary>
		 * <param name="key"></param>
		 * <param name="value"></param>
		 * <returns>The old value if it was present.</returns>
		 * <remarks>The key is not updated. This may matter for types that can be equal without being identical.</remarks>
		 */
		Optional<V> Insert(const K&  key, const V&  value) { return insert(          key ,           value ); }
		Optional<V> Insert(const K&  key,       V&& value) { return insert(          key , std::move(value)); }
		Optional<V> Insert(      K&& key, const V&  value) { return insert(std::move(key),           value ); }
		Optional<V> Insert(      K&& key,       V&& value) { return insert(std::move(key), std::move(value)); }

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
		Optional<V> Remove(const K&  key) { return remove(key); }
		Optional<V> Remove(      K&& key) { return remove(std::move(key)); }

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
		Optional<V&> Get(const K&  key) { return get(key); }
		Optional<V&> Get(      K&& key) { return get(std::move(key)); }
		Optional<const V&> Get(const K&  key) const { return get(key); }
		Optional<const V&> Get(      K&& key) const { return get(std::move(key)); }

		const V& operator[](const K& key) const { return Get(key).Value(); }
		      V& operator[](const K& key)       { return Get(key).Value(); }

		/// <returns>The number of elements in the map.</returns>
		size_t GetSize() const { return len; };
		/// <returns>True if the map contains no elements.</returns>
		bool IsEmpty() const { return GetSize() == 0; };

		/// <summary>Clears the map, removing all elements. Frees all memory except the root node.</summary>
		void Clear() {
			if (root.node == nullptr) {
				return;
			}

			KVERef current = BTree::first_leaf_edge(root.as_node_ref());
			while (len) {
				len -= 1;
				if (current.idx < current.node_ref.node->len) {
					current = KVERef{current.node_ref, current.idx + 1};
					continue;
				} else {
					const NodeRef to_delete = current.node_ref;
					current = current.node_ref.ascend().parent;
					delete to_delete.node;
				}

				for (;;) {
					if (current.idx < current.node_ref.node->len) {
						current = BTree::first_leaf_edge(KVERef{current.node_ref, current.idx + 1}.descend());
						break; //continue outer loop
					} else {
						const NodeRef to_delete = current.node_ref;
						current = current.node_ref.ascend().parent;
						delete to_delete.node->as_branch();
					}
				}
			}

			LeafNode* new_root_to_be = current.node_ref.node;

			auto ascension = current.node_ref.ascend();
			while (ascension.succeeded) {
				KVERef node = ascension.parent;
				ascension = node.node_ref.ascend();
				delete node.node_ref.node->as_branch();
			}

			//reinitialize the root node
			new_root_to_be->~LeafNode();
			::new(new_root_to_be) LeafNode();

			root = {new_root_to_be, 0};
		}

		ConstIterator cbegin() const { return {BTree::first_leaf_edge(const_cast<OrderedMap&>(*this).root.as_node_ref()), 0, GetSize()}; }
		ConstIterator cend() const {
			if (root.node == nullptr) {
				return cbegin();
			}
			return {BTree::last_leaf_edge(const_cast<OrderedMap&>(*this).root.as_node_ref()), GetSize(), GetSize()};
		}

		ConstIterator begin() const { return cbegin(); }
		ConstIterator end()   const { return cend();   }

		Iterator begin() { return {BTree::first_leaf_edge(root.as_node_ref()), 0, GetSize()}; }
		Iterator end() {
			if (root.node == nullptr) {
				return begin();
			}
			return {BTree::last_leaf_edge(root.as_node_ref()), GetSize(), GetSize()};
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
		MapEntry<Key&&> entry(Key&& key) {
			const auto search_result = search_tree(root.as_node_ref(), key);
			switch (search_result.result) {
				case SearchResult::FOUND:  return MapEntry<Key&&>::Occupied(search_result.handle, len);
				case SearchResult::FAILED: return MapEntry<Key&&>::Vacant(  search_result.handle, len, std::forward<Key>(key));
				default: UNREACHABLE();
			}
		}

		template<typename Key, typename Val>
		Optional<V> insert(Key&& key, Val&& value) {
			auto entry = Entry(std::forward<Key>(key));
			if (entry.IsVacant()) {
				entry.VacantInsert(std::forward<Val>(value));
				return {};
			} else {
				return {entry.OccupiedReplace(std::forward<Val>(value))};
			}
		}

		template<typename Key>
		Optional<V> remove(Key&& key) {
			auto entry = Entry(std::forward<Key>(key));
			if (!entry.IsVacant()) {
				return {entry.Remove()};
			} else {
				return {};
			}
		}

		template<typename Key>
		Optional<const V&> get(Key&& key) const {
			auto entry = const_cast<OrderedMap&>(*this).Entry(std::forward<Key>(key)); //note(vuko): using only const pathways down the line
			if (!entry.IsVacant()) {
				return {entry.OccupiedGet()};
			} else {
				return {};
			}
		}

		template<typename Key>
		Optional<V&> get(Key&& key) {
			auto entry = Entry(std::forward<Key>(key));
			if (!entry.IsVacant()) {
				return {entry.OccupiedGet()};
			} else {
				return {};
			}
		}

		/// <see cref="OrderedMap::Entry()"/>
		template<typename Key>
		class MapEntry {
		public:
			static MapEntry Vacant  (KVERef kv_ref, size_t& len, Key&& key) { return MapEntry(kv_ref, len, std::forward<Key>(key)); }
			static MapEntry Occupied(KVERef kv_ref, size_t& len)            { return MapEntry(kv_ref, len); }

			MapEntry(MapEntry&&) = default;

			/// <returns>True if the entry is vacant.</returns>
			bool IsVacant() const { return static_cast<bool>(key); }
			operator bool() const { return !IsVacant(); }

			/**
			 * <summary>Ensures a value is in the map by inserting if vacant.</summary>
			 * <param name="value">A value to insert if the entry is vacant.</param>
			 * <returns>Reference to the value in the map.</returns>
			 */
			V& OrInsert(const V&  value) { return or_insert(value); }
			V& OrInsert(      V&& value) { return or_insert(std::move(value)); }

			/**
			 * <summary>Inserts the value into the map at entry's key.</summary>
			 * <param name="value"></param>
			 * <returns>Reference to the value in the map.</returns>
			 */
			V& VacantInsert(const V&  value) { return vacant_insert(value); }
			V& VacantInsert(      V&& value) { return vacant_insert(std::move(value)); }

			/**
			 * <summary>Replaces the value in the map at entry's key.</summary>
			 * <param name="value"></param>
			 * <returns>Old value.</returns>
			 */
			V OccupiedReplace(const V&  value) { return occupied_replace(value); }
			V OccupiedReplace(      V&& value) { return occupied_replace(std::move(value)); }

			/// <returns>A reference to the value in the entry.</returns>
			const V& OccupiedGet() const { ASSERTE(!IsVacant(), "Cannot read value from a vacant map entry!"); return kv_ref.node_ref.node->values[kv_ref.idx]; }
			V& OccupiedGet() { return const_cast<V&>(const_cast<const MapEntry&>(*this).OccupiedGet()); }

			/// <summary>A key-value pair.</summary>
			struct kv {
				K key;
				V value;
			};
			/**
			 * <summary>Removes the entry from the map.</summary>
			 * <returns>Removed key-value pair.</returns>
			 */
			kv RemoveKV() {
				ASSERTE(!IsVacant(), "Cannot remove value from a vacant map entry!");

				map_len -= 1;

				auto handle_underflow = [](NodeRef cur_node) {
					while (cur_node.node->len < BTree::MIN_LEN) {
						const auto ascension = cur_node.ascend();
						if (!ascension.succeeded) {
							break; //root is allowed to underflow
						}

						const KVERef parent = ascension.parent;
						bool is_left;
						KVERef handle;
						if (parent.idx > 0) {
							is_left = true;
							handle = {parent.node_ref, parent.idx - 1};
						} else {
							is_left = false;
							handle = {parent.node_ref, parent.idx};
						}

						if (!handle.can_merge()) {
							if (is_left) {
								handle.steal_left();
							} else {
								handle.steal_right();
							}
							break;
						}

						NodeRef node_ref = handle.merge().node_ref;
						if (node_ref.node->len == 0) {
							//a node of zero length; only root is allowed to underflow, therefore we must be at root
							//the root is empty with only one child leaf, get rid of it
							node_ref.root->pop_level();
							break;
						}

						cur_node = node_ref;
					}
				};

				if(kv_ref.node_ref.height == 0) {
					//leaf
					const auto remove_result = kv_ref.leaf_remove();
					handle_underflow(kv_ref.node_ref);
					return kv{std::move(remove_result.k), std::move(remove_result.v)};
				} else {
					//branch
					auto& key_ref = kv_ref.node_ref.node->keys  [kv_ref.idx];
					auto& val_ref = kv_ref.node_ref.node->values[kv_ref.idx];

					auto to_remove = BTree::first_leaf_edge(KVERef{kv_ref.node_ref, kv_ref.idx + 1}.descend());
					auto remove_result = to_remove.leaf_remove();

					std::swap(key_ref, remove_result.k);
					std::swap(val_ref, remove_result.v);

					handle_underflow(to_remove.node_ref);
					return kv{std::move(remove_result.k), std::move(remove_result.v)};
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
			V& or_insert(Val&& value) {
				if (key) {
					return VacantInsert(std::forward<Val>(value));
				} else {
					return OccupiedGet();
				}
			}

			template<typename Val>
			V& vacant_insert(Val&& value) {
				ASSERTE(IsVacant(), "Cannot insert into already occupied map entry!");

				if (kv_ref.node_ref.node == nullptr) {
					//we've got nothing to operate on; plant the little happy tree first!
					auto sapling = new LeafNode();
					kv_ref.node_ref.node       = sapling;
					kv_ref.node_ref.root->node = sapling;
				}

				map_len += 1;

				auto insert_result = kv_ref.leaf_insert(key.TakeValue(), std::forward<Val>(value));
				if (insert_result.fit) {
					return *insert_result.value_ref;
				}
				//insertion resulted in a node split; need to re-insert the extracted key-value pair

				V* v = insert_result.value_ref; //points to the value we just inserted
				auto ascension_result = insert_result.split_result.old_node_left.ascend();

				for (;;) {
					if (ascension_result.succeeded) {
						insert_result = ascension_result.parent.branch_insert(insert_result.split_result.key, insert_result.split_result.value, insert_result.split_result.new_edge_right);
						if (insert_result.fit) {
							return *v;
						}
						ascension_result = insert_result.split_result.old_node_left.ascend();
					} else {
						Root* root = ascension_result.self.root;
						root->push_level().push_back(insert_result.split_result.key, insert_result.split_result.value, insert_result.split_result.new_edge_right);
						return *v;
					}
				}
			}

			template<typename Val>
			V occupied_replace(Val&& value) {
				ASSERTE(!IsVacant(), "Cannot replace (non-existent) value in a vacant map entry!");
				auto old = std::move(OccupiedGet());
				OccupiedGet() = std::forward<Val>(value);
				return old;
			}

			MapEntry(KVERef kv_ref, size_t& len, Key&& key) : key(std::forward<Key>(key)), kv_ref(kv_ref), map_len(len) {}
			MapEntry(KVERef kv_ref, size_t& len)            : key(),                       kv_ref(kv_ref), map_len(len) {}

			Optional<Key> key;
			KVERef  kv_ref;
			size_t& map_len;
		};

		struct ConstKV {
			const K& key;
			const V& value;
			const ConstKV* operator->() const { return this; } //note(vuko): needed by the iterator's own operator ->, since it returns KV by value
		};
		struct KV {
			const K& key;
			V& value;
			KV* operator->() { return this; }
		};

		template<typename RetKV>
		class IteratorBase : public std::iterator<std::bidirectional_iterator_tag, RetKV> {
		public:
			IteratorBase(KVERef kv_ref, size_t position, size_t map_len) : current(kv_ref), next_forward(kv_ref), next_backward(kv_ref), position(position), map_len(map_len) {
					if (position != map_len) {
						if (kv_ref.idx < kv_ref.node_ref.node->len) {
							next_forward = KVERef{kv_ref.node_ref, kv_ref.idx + 1};
						}
					}
				}

			bool operator==(const IteratorBase& other) const { return this->current == other.current; }
			bool operator!=(const IteratorBase& other) const { return !(*this == other); }

			IteratorBase& operator++() { next_backward = current; current = next(); return *this; }
			IteratorBase& operator--() { next_forward  = current; current = prev(); return *this; }
			IteratorBase operator++(int) { IteratorBase ret(current, position, map_len); operator++(); return ret; }
			IteratorBase operator--(int) { IteratorBase ret(current, position, map_len); operator--(); return ret; }
		private:
			KVERef next() {
				KVERef kv_ref = next_forward;

				if (position + 1u >= map_len) {
					return kv_ref;
				}
				position += 1u;

				if (kv_ref.idx < kv_ref.node_ref.node->len) {
					next_forward = KVERef{kv_ref.node_ref, kv_ref.idx + 1u};
					return kv_ref;
				}

				do {
					kv_ref = kv_ref.node_ref.ascend().parent;
				} while (kv_ref.idx >= kv_ref.node_ref.node->len);

				next_forward = BTree::first_leaf_edge(KVERef{kv_ref.node_ref, kv_ref.idx + 1u}.descend());
				return kv_ref;
			}
			KVERef prev() {
				KVERef kv_ref = next_backward;

				if (position <= 1u) {
					next_forward = KVERef{current.node_ref, current.idx + 1 - position};
					position = 0;
					return kv_ref;
				}
				position -= 1u;

				if (kv_ref.idx > 0) {
					next_backward = KVERef{kv_ref.node_ref, kv_ref.idx - 1u};
					return kv_ref;
				}

				do {
					kv_ref = kv_ref.node_ref.ascend().parent;
				} while (kv_ref.idx == 0);

				next_backward = BTree::last_leaf_edge(KVERef{kv_ref.node_ref, kv_ref.idx - 1u}.descend());
				return kv_ref;
			}
		protected:
			KVERef current;
			KVERef next_forward;
			KVERef next_backward;
			size_t position;
			size_t map_len;
		};

		class ConstIterator : public IteratorBase<ConstKV> {
			using IteratorBase<ConstKV>::current;
		public:
			using IteratorBase<ConstKV>::IteratorBase;
			ConstKV operator*()  const { return {current.node_ref.node->keys[current.idx], current.node_ref.node->values[current.idx]}; }
			ConstKV operator->() const { return operator*(); }
		};

		class Iterator : public IteratorBase<KV> {
			using IteratorBase<KV>::current;
		public:
			using IteratorBase<KV>::IteratorBase;
			KV operator*()  { return {current.node_ref.node->keys[current.idx], current.node_ref.node->values[current.idx]}; }
			KV operator->() { return operator*(); }
		};

		/// <see cref="OrderedMap::Keys()"/>
		class Keys {
			class Iter;
		public:
			Keys(const OrderedMap& map) : map(map) {}

			Iter cbegin() const { return {map.cbegin()}; }
			Iter cend()   const { return {map.cend()  }; }
			Iter begin()  const { return cbegin(); }
			Iter end()    const { return cend(); }
		private:
			using ParentIter = typename OrderedMap::ConstIterator;
			class Iter : public std::iterator<std::bidirectional_iterator_tag, K> {
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
		class ConstValues {
			class Iter;
		public:
			ConstValues(const OrderedMap& map) : map(map) {}

			Iter cbegin() const { return {map.cbegin()}; }
			Iter cend()   const { return {map.cend()}; }
			Iter begin()  const { return cbegin(); }
			Iter end()    const { return cend(); }
		private:
			using ParentIter = typename OrderedMap::ConstIterator;
			class Iter : public std::iterator<std::bidirectional_iterator_tag, V> {
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
		class Values {
			class Iter;
		public:
			Values(OrderedMap& map) : map(map) {}

			Iter begin() { return {map.begin()}; }
			Iter end()   { return {map.end()}; }
		private:
			using ParentIter = typename OrderedMap::Iterator;
			class Iter : public std::iterator<std::bidirectional_iterator_tag, V> {
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

		struct SearchResult {
			enum { FOUND, DESCEND, FAILED = DESCEND } result;
			KVERef handle;
		};

		static SearchResult search_tree(NodeRef node_ref, const K& key) {
			if (node_ref.node == nullptr) {
				return {SearchResult::FAILED, KVERef{node_ref, 0}};
			}

			for (;;) {
				const auto search_result = search_node(node_ref, key);
				switch (search_result.result) {
					case SearchResult::FOUND: return {SearchResult::FOUND, search_result.handle};
					case SearchResult::DESCEND: {
						const auto handle = search_result.handle;
						if (handle.node_ref.height == 0) {
							//leaf
							return {SearchResult::FAILED, handle};
						}
						//branch
						node_ref = handle.descend();
					}
				}
			}
		}

		static SearchResult search_node(const NodeRef node_ref, const K& key) {
			return search_linear(node_ref, key);
			//todo(vuko): possibly switch to binary search when B is large enough (how large?)
		}

		template<typename T> static auto equality_check(const T& a, const T& b, int) -> decltype(a == b) { return a == b; } //note(vuko): templating and decltype are used for SFINAE here
		template<typename T> static auto equality_check(const T& a, const T& b, ...) -> decltype(bool{}) { return !(a < b || b < a); }
		static bool equals(const K& a, const K& b) { constexpr int choose{}; return equality_check(a, b, choose); }

		static SearchResult search_linear(const NodeRef node_ref, const K& search_key) {
			const LeafNode* const node = node_ref.node;
			for (size_t i = 0; i < node->len; i++) {
				const K& tested_key = node->keys[i];
				if (search_key < tested_key) {
					return {SearchResult::DESCEND, KVERef{node_ref, i}};
				} else if (equals(search_key, tested_key)) {
					return {SearchResult::FOUND, KVERef{node_ref, i}};
				}
			}
			return {SearchResult::DESCEND, KVERef{node_ref, node->len}};
		}

		static SearchResult search_binary(NodeRef node_ref, const K& search_key);

		Root root;
		size_t len;
	};

} //namespace Poly

template<typename K, typename V, size_t B> void swap(Poly::OrderedMap<K, V, B>& lhs, Poly::OrderedMap<K, V, B>& rhs) { lhs.Swap(rhs); };
