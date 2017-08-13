#pragma once

#include "BTreeNode.hpp"
#include "Optional.hpp"

namespace Poly {
	//todo(vuko): docs
	template<typename K, typename V, size_t Bfactor = 6>
	class BTreeMap {
		static_assert(Bfactor > 1, "B factor must be greater than 1. Consider using a classic binary tree if you need a lesser value.");
		using BTree    = Impl::BTree<K, V, Bfactor>;
		using Root     = typename BTree::Root;
		using LeafNode = typename BTree::LeafNode;
		using KVERef   = typename BTree::KVERef;
		using NodeRef  = typename BTree::NodeRef;
		class Entry;
		class ConstIterator;
		class Iterator;
		class Keys; //note(vuko): key references are always const; allowing to modify them would break the order invariant
		class ConstValues;
		class Values;
	public:
		static constexpr size_t B = Bfactor;

	public:
		BTreeMap() : root{nullptr, 0}, len(0) {}
		BTreeMap(BTreeMap&& other) : root(other.root), len(other.len) { ::new(&other) BTreeMap(); }
		BTreeMap(const BTreeMap& other) : BTreeMap() { for (auto kv : other) { this->Insert(kv.key, kv.value); } } //todo(vuko): can be implemented more efficiently
		~BTreeMap() { if (this->root.node) { this->Clear(); delete this->root.node; } };

	public:
		BTreeMap& operator=(BTreeMap&& other) {
			this->~BTreeMap();
			this->root = other.root;
			this->len  = other.len;
			::new(&other) BTreeMap();
			return *this;
		}
		BTreeMap& operator=(const BTreeMap& other) { this->~BTreeMap(); ::new(this) BTreeMap(other); return *this; }

	public:
		Entry Entry(const K& key) {
			if (this->root.node == nullptr) {
				return Entry::Vacant(KVERef{this->root.as_node_ref(), 0}, this->len, key);
			}

			const auto search_result = search_tree(this->root.as_node_ref(), key);
			switch (search_result.result) {
				case SearchResult::FOUND:  return Entry::Occupied(search_result.handle, this->len);
				case SearchResult::FAILED: return Entry::Vacant(  search_result.handle, this->len, key);
				default: UNREACHABLE();
			}
		}

		Optional<V> Insert(const K&  key, const V&  value) { return this->insert(          key ,           value ); }
		Optional<V> Insert(const K&  key,       V&& value) { return this->insert(          key , std::move(value)); }
		Optional<V> Insert(      K&& key, const V&  value) { return this->insert(std::move(key),           value ); }
		Optional<V> Insert(      K&& key,       V&& value) { return this->insert(std::move(key), std::move(value)); }

		Optional<V> Remove(const K&  key) { return this->remove(key); }
		Optional<V> Remove(      K&& key) { return this->remove(std::move(key)); }

		Optional<V&> Get(const K&  key) { return this->get(key); }
		Optional<V&> Get(      K&& key) { return this->get(std::move(key)); }
		Optional<const V&> Get(const K&  key) const { return this->get(key); }
		Optional<const V&> Get(      K&& key) const { return this->get(std::move(key)); }

		const V& operator[](const K& key) const { return this->Get(key).Value(); }
		      V& operator[](const K& key)       { return this->Get(key).Value(); }

		size_t GetSize() const { return this->len; };
		bool   IsEmpty() const { return this->GetSize() == 0; };

		void Clear() {
			if (this->root.node == nullptr) {
				return;
			}

			KVERef current = BTree::first_leaf_edge(this->root.as_node_ref());
			while (this->len) {
				this->len -= 1;
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

			this->root = {new_root_to_be, 0};
		}

	public:
		ConstIterator cbegin() const { return {BTree::first_leaf_edge(const_cast<BTreeMap&>(*this).root.as_node_ref()), 0, this->GetSize()}; }
		ConstIterator cend() const {
			if (this->root.node == nullptr) {
				return this->cbegin();
			}
			return {BTree::last_leaf_edge(const_cast<BTreeMap&>(*this).root.as_node_ref()), this->GetSize(), this->GetSize()};
		}

		ConstIterator begin() const { return this->cbegin(); }
		ConstIterator end()   const { return this->cend();   }

		Iterator begin() { return {BTree::first_leaf_edge(this->root.as_node_ref()), 0, this->GetSize()}; }
		Iterator end() {
			if (this->root.node == nullptr) {
				return this->begin();
			}
			return {BTree::last_leaf_edge(this->root.as_node_ref()), this->GetSize(), this->GetSize()};
		}

		class Keys   Keys()   const { return {*this}; }
		class Values Values()       { return {*this}; }
		ConstValues  Values() const { return {*this}; }

		void Swap(BTreeMap& other) { std::swap(this->root, other.root); std::swap(this->len, other.len); }

	private:
		template<typename Key, typename Val>
		Optional<V> insert(Key&& key, Val&& value) {
			auto entry = this->Entry(std::forward<Key>(key));
			if (entry.IsVacant()) {
				entry.VacantInsert(std::forward<Val>(value));
				return {};
			} else {
				return {entry.OccupiedInsert(std::forward<Val>(value))};
			}
		}

		template<typename Key>
		Optional<V> remove(Key&& key) {
			auto entry = this->Entry(std::forward<Key>(key));
			if (!entry.IsVacant()) {
				return {entry.Remove()};
			} else {
				return {};
			}
		}

		template<typename Key>
		Optional<const V&> get(Key&& key) const {
			auto entry = const_cast<BTreeMap&>(*this).Entry(std::forward<Key>(key)); //note(vuko): using only const pathways down the line
			if (!entry.IsVacant()) {
				return {entry.OccupiedGet()};
			} else {
				return {};
			}
		}

		template<typename Key>
		Optional<V&> get(Key&& key) {
			auto entry = this->Entry(std::forward<Key>(key));
			if (!entry.IsVacant()) {
				return {entry.OccupiedGet()};
			} else {
				return {};
			}
		}

	private:
		class Entry {
		public:
			static Entry Vacant  (KVERef kv_ref, size_t& len, const K&  key) { return Entry(kv_ref, len, key); }
			static Entry Vacant  (KVERef kv_ref, size_t& len,       K&& key) { return Entry(kv_ref, len, std::move(key)); }
			static Entry Occupied(KVERef kv_ref, size_t& len)                { return Entry(kv_ref, len); }

		public:
			Entry(Entry&&) = default;

		public:
			bool IsVacant() const { return static_cast<bool>(key); }
			operator bool() const { return !this->IsVacant(); }

			V& OrInsert(const V&  value) { return this->or_insert(value); }
			V& OrInsert(      V&& value) { return this->or_insert(std::move(value)); }

			V& VacantInsert(const V&  value) { return this->vacant_insert(value); }
			V& VacantInsert(      V&& value) { return this->vacant_insert(std::move(value)); }

			V OccupiedInsert(const V&  value) { return this->occupied_insert(value); }
			V OccupiedInsert(      V&& value) { return this->occupied_insert(std::move(value)); }

			const V& OccupiedGet() const { ASSERTE(!this->IsVacant(), "Cannot read value from a vacant map entry!"); return this->kv_ref.node_ref.node->values[this->kv_ref.idx]; }
			V& OccupiedGet() { return const_cast<V&>(const_cast<const Entry&>(*this).OccupiedGet()); }

			struct kv {
				K key;
				V value;
			};
			kv RemoveKV() {
				ASSERTE(!this->IsVacant(), "Cannot remove value from a vacant map entry!");

				this->map_len -= 1;

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

				if(this->kv_ref.node_ref.height == 0) {
					//leaf
					const auto remove_result = this->kv_ref.leaf_remove();
					handle_underflow(this->kv_ref.node_ref);
					return kv{std::move(remove_result.k), std::move(remove_result.v)};
				} else {
					//branch
					auto& key_ref = this->kv_ref.node_ref.node->keys  [this->kv_ref.idx];
					auto& val_ref = this->kv_ref.node_ref.node->values[this->kv_ref.idx];

					auto to_remove = BTree::first_leaf_edge(KVERef{this->kv_ref.node_ref, this->kv_ref.idx + 1}.descend());
					auto remove_result = to_remove.leaf_remove();

					std::swap(key_ref, remove_result.k);
					std::swap(val_ref, remove_result.v);

					handle_underflow(to_remove.node_ref);
					return kv{std::move(remove_result.k), std::move(remove_result.v)};
				}
			}

			V Remove() { return this->RemoveKV().value; }

		private:
			template<typename Val>
			V& or_insert(Val&& value) {
				if (this->key) {
					return this->VacantInsert(std::forward<Val>(value));
				} else {
					return this->OccupiedGet();
				}
			}

			template<typename Val>
			V& vacant_insert(Val&& value) {
				ASSERTE(this->IsVacant(), "Cannot insert into already occupied map entry!");

				if (this->kv_ref.node_ref.node == nullptr) {
					//we've got nothing to operate on; plant the little happy tree first!
					auto sapling = new LeafNode();
					kv_ref.node_ref.node       = sapling;
					kv_ref.node_ref.root->node = sapling;
				}

				this->map_len += 1;

				auto insert_result = this->kv_ref.leaf_insert(this->key.TakeValue(), std::forward<Val>(value));
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
			V occupied_insert(Val&& value) {
				ASSERTE(!this->IsVacant(), "Cannot replace (non-existent) value in a vacant map entry!");
				//std::swap(value, this->OccupiedGet());
				//return value;
				auto old = std::move(this->OccupiedGet());
				this->OccupiedGet() = std::forward<Val>(value);
				return old;
			}

		private:
			Entry(KVERef kv_ref, size_t& len, const K&  key) : key(key),            kv_ref(kv_ref), map_len(len) {}
			Entry(KVERef kv_ref, size_t& len,       K&& key) : key(std::move(key)), kv_ref(kv_ref), map_len(len) {}
			Entry(KVERef kv_ref, size_t& len)                : key{},               kv_ref(kv_ref), map_len(len) {}

		private:
			Optional<K> key; //todo(vuko): store a reference instead (need to parametrize the class on reference type; lvalue-ref -> opt<T&>, rvalue-ref -> opt<T>)
			KVERef kv_ref;
			size_t& map_len;
		};

	private:
		struct ConstKV {
			const K& key;
			const V& value;
			const ConstKV* operator->() const { return this; } //note(vuko): needed by the iterator's own operator ->, since it returns KV by value
		};
		struct KV {
			const K& key;
			V& value;
			KV* operator->()       { return this; }
		};

		template<typename RetKV>
		class IteratorBase : public std::iterator<std::bidirectional_iterator_tag, RetKV> {
		public:
			IteratorBase(KVERef kv_ref, size_t position, size_t map_len) : current(kv_ref), next_forward(kv_ref), next_backward(kv_ref), position(position), map_len(map_len) {
					if (this->position != this->map_len) {
						if (kv_ref.idx < kv_ref.node_ref.node->len) {
							this->next_forward = KVERef{kv_ref.node_ref, kv_ref.idx + 1};
						}
					}
				}
		public:
			bool operator==(const IteratorBase& other) const { return this->current == other.current; }
			bool operator!=(const IteratorBase& other) const { return !(*this == other); }
		public:
			IteratorBase& operator++() { this->next_backward = this->current; this->current = this->next(); return *this; }
			IteratorBase& operator--() { this->next_forward  = this->current; this->current = this->prev(); return *this; }
			IteratorBase operator++(int) { IteratorBase ret(this->current, this->position, this->map_len); operator++(); return ret; }
			IteratorBase operator--(int) { IteratorBase ret(this->current, this->position, this->map_len); operator--(); return ret; }
		private:
			KVERef next() {
				KVERef kv_ref = this->next_forward;

				if (this->position + 1u >= this->map_len) {
					return kv_ref;
				}
				this->position += 1u;

				if (kv_ref.idx < kv_ref.node_ref.node->len) {
					this->next_forward = KVERef{kv_ref.node_ref, kv_ref.idx + 1u};
					return kv_ref;
				}

				do {
					kv_ref = kv_ref.node_ref.ascend().parent;
				} while (kv_ref.idx >= kv_ref.node_ref.node->len);

				this->next_forward = BTree::first_leaf_edge(KVERef{kv_ref.node_ref, kv_ref.idx + 1u}.descend());
				return kv_ref;
			}
			KVERef prev() {
				KVERef kv_ref = this->next_backward;

				if (this->position <= 1u) {
					this->next_forward = KVERef{this->current.node_ref, this->current.idx + 1 - this->position};
					this->position = 0;
					return kv_ref;
				}
				this->position -= 1u;

				if (kv_ref.idx > 0) {
					this->next_backward = KVERef{kv_ref.node_ref, kv_ref.idx - 1u};
					return kv_ref;
				}

				do {
					kv_ref = kv_ref.node_ref.ascend().parent;
				} while (kv_ref.idx == 0);

				this->next_backward = BTree::last_leaf_edge(KVERef{kv_ref.node_ref, kv_ref.idx - 1u}.descend());
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
			ConstKV operator*()  const { return {this->current.node_ref.node->keys[this->current.idx], this->current.node_ref.node->values[this->current.idx]}; }
			ConstKV operator->() const { return operator*(); }
		};

		class Iterator : public IteratorBase<KV> {
			using IteratorBase<KV>::current;
		public:
			using IteratorBase<KV>::IteratorBase;
			KV operator*()  { return {this->current.node_ref.node->keys[this->current.idx], this->current.node_ref.node->values[this->current.idx]}; }
			KV operator->() { return operator*(); }
		};

		class Keys {
			class Iter;
		public:
			Keys(const BTreeMap& map) : map(map) {}
		public:
			Iter cbegin() const { return {this->map.cbegin()}; }
			Iter cend()   const { return {this->map.cend()  }; }
			Iter begin()  const { return this->cbegin(); }
			Iter end()    const { return this->cend(); }
		private:
			using ParentIter = typename BTreeMap::ConstIterator;
			class Iter : public std::iterator<std::bidirectional_iterator_tag, K> {
			public:
				Iter(ParentIter iter) : iter(iter) {}
			public:
				bool operator==(const Iter& other) const { return this->iter == other.iter; }
				bool operator!=(const Iter& other) const { return !(*this == other); }
			public:
				const K& operator*()  { return this->iter->key; }
				const K& operator->() { return operator*(); }
			public:
				Iter& operator++() { ++(this->iter); return *this; }
				Iter& operator--() { --(this->iter); return *this; }
				Iter operator++(int) { Iter ret(this->iter); operator++(); return ret; }
				Iter operator--(int) { Iter ret(this->iter); operator--(); return ret; }
			private:
				ParentIter iter;
			};
		private:
			const BTreeMap& map;
		};

		class ConstValues {
			class Iter;
		public:
			ConstValues(const BTreeMap& map) : map(map) {}
		public:
			Iter cbegin() const { return {this->map.cbegin()}; }
			Iter cend()   const { return {this->map.cend()}; }
			Iter begin()  const { return this->cbegin(); }
			Iter end()    const { return this->cend(); }
		private:
			using ParentIter = typename BTreeMap::ConstIterator;
			class Iter : public std::iterator<std::bidirectional_iterator_tag, V> {
			public:
				Iter(ParentIter iter) : iter(iter) {}
			public:
				bool operator==(const Iter& other) const { return this->iter == other.iter; }
				bool operator!=(const Iter& other) const { return !(*this == other); }
			public:
				Iter& operator++() { ++(this->iter); return *this; }
				Iter& operator--() { --(this->iter); return *this; }
				Iter operator++(int) { Iter ret(this->iter); operator++(); return ret; }
				Iter operator--(int) { Iter ret(this->iter); operator--(); return ret; }
			public:
				const V& operator*()  { return this->iter->value; }
				const V& operator->() { return operator*(); }
			private:
				ParentIter iter;
			};
		private:
			const BTreeMap& map;
		};

		class Values {
			class Iter;
		public:
			Values(BTreeMap& map) : map(map) {}
		public:
			Iter begin() { return {this->map.begin()}; }
			Iter end()   { return {this->map.end()}; }
		private:
			using ParentIter = typename BTreeMap::Iterator;
			class Iter : public std::iterator<std::bidirectional_iterator_tag, V> {
			public:
				Iter(ParentIter iter) : iter(iter) {}
			public:
				bool operator==(const Iter& other) const { return this->iter == other.iter; }
				bool operator!=(const Iter& other) const { return !(*this == other); }
			public:
				Iter& operator++() { ++(this->iter); return *this; }
				Iter& operator--() { --(this->iter); return *this; }
				Iter operator++(int) { Iter ret(this->iter); operator++(); return ret; }
				Iter operator--(int) { Iter ret(this->iter); operator--(); return ret; }
			public:
				V& operator*()  { return this->iter->value; }
				V& operator->() { return operator*(); }
			private:
				ParentIter iter;
			};
		private:
			BTreeMap& map;
		};

	private:
		struct SearchResult {
			enum { FOUND, DESCEND, FAILED = DESCEND } result;
			KVERef handle;
		};

		static SearchResult search_tree(NodeRef node_ref, const K& key) {
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

	private:
		Root root;
		size_t len;
	};

} //namespace Poly

template<typename K, typename V, size_t B> void swap(Poly::BTreeMap<K, V, B>& lhs, Poly::BTreeMap<K, V, B>& rhs) { lhs.Swap(rhs); };
