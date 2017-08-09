#pragma once

#include "BTreeNode.hpp"
#include "Optional.hpp"

namespace Poly {

	template<typename K, typename V, size_t B>
	struct Range { //note(vuko): doubles as a proper fucking iterator; behold the goddamn beauty! (alright, this is for show only for now)
		using BTree  = Impl::BTree<K, V, B>;
		using KVERef = typename BTree::KVERef;

		KVERef front;
		KVERef back;

		struct KV {
			const K& key;
			V& value;
		};

		KV next_unchecked() {
			KVERef kv_ref = this->front;

			if (kv_ref.idx < kv_ref.node_ref.node->len) {
				front = KVERef{kv_ref.node_ref, kv_ref.idx + 1};
				return {kv_ref.node_ref.node->keys[kv_ref.idx], kv_ref.node_ref.node->values[kv_ref.idx]};
			}

			do {
				kv_ref = kv_ref.node_ref.ascend().parent;
			} while (kv_ref.idx >= kv_ref.node_ref.node->len);

			front = BTree::first_leaf_edge(KVERef{kv_ref.node_ref, kv_ref.idx + 1}.descend());
			return {kv_ref.node_ref.node->keys[kv_ref.idx], kv_ref.node_ref.node->values[kv_ref.idx]};
		}

		KV prev_unchecked() {
			KVERef kv_ref = this->back;

			if (kv_ref.idx > 0) {
				this->back = KVERef{kv_ref.node_ref, kv_ref.idx - 1};
				return {kv_ref.node_ref.node->keys[kv_ref.idx], kv_ref.node_ref.node->values[kv_ref.idx]};
			}

			do {
				kv_ref = kv_ref.node_ref.ascend().parent;
			} while (kv_ref.idx == 0);

			this->back = BTree::last_leaf_edge(KVERef{kv_ref.node_ref, kv_ref.idx - 1}.descend());
			return {kv_ref.node_ref.node->keys[kv_ref.idx], kv_ref.node_ref.node->values[kv_ref.idx]};
		}

		KV next() {
			assert(!(front == back));
			return next_unchecked();
		}
	};

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
		class Iter;
		class Keys;
		class Values;
	public:
		static constexpr size_t B = Bfactor;

	public:
		BTreeMap() : root{new LeafNode(), 0}, len(0) {} //todo(vuko): should we delay the allocation until the first insertion?
		BTreeMap(BTreeMap&& other) : root(other.root), len(other.len) { ::new(&other) BTreeMap(); } //note(vuko): due to the previous issue this allocates!
		//todo(vuko): copy constructor? tbh I'd rather avoid implementing it
		~BTreeMap() { this->Clear(); delete this->root.node; };

	public:
		BTreeMap& operator=(BTreeMap&& other) {
			this->~BTreeMap();
			this->root = other.root;
			this->len = other.len;
			::new(&other) BTreeMap();
			return *this;
		}

	public:
		Optional<V> Insert(const K&  key, const V&  value) { return this->insert(          key ,           value ); }
		Optional<V> Insert(const K&  key,       V&& value) { return this->insert(          key , std::move(value)); }
		Optional<V> Insert(      K&& key, const V&  value) { return this->insert(std::move(key),           value ); }
		Optional<V> Insert(      K&& key,       V&& value) { return this->insert(std::move(key), std::move(value)); }

		Optional<V> Remove(const K&  key) { return this->remove(key); }
		Optional<V> Remove(      K&& key) { return this->remove(std::move(key)); }

		Optional<V&> Get(const K&  key) { return this->get(key); }
		Optional<V&> Get(      K&  key) { return this->get(std::move(key)); }

		V& operator[](const K& key) { return this->Get(key).value(); }
		size_t GetSize() { return this->len; };
		bool IsEmpty()   { return this->GetSize() == 0; };

		void Clear() {
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
		Entry Entry(const K& key) {
			const auto search_result = search_tree(this->root.as_node_ref(), key);
			switch (search_result.result) {
				case SearchResult::FOUND:  return Entry::Occupied(search_result.handle, this->len);
				case SearchResult::FAILED: return Entry::Vacant(  search_result.handle, this->len, key);
				default: UNREACHABLE();
			}
		}
		Range<K, V, Bfactor> MaxRange() { return {BTree::first_leaf_edge(this->root.as_node_ref()), BTree::last_leaf_edge(this->root.as_node_ref())}; }
		Range<K, V, Bfactor> GetRange(const K& lower, const K& upper) { ASSERTE(lower < upper, ""); return {search_tree(this->root.as_node_ref(), lower).handle, search_tree(this->root.as_node_ref(), upper).handle}; }
		Iter begin() { return {BTree::first_leaf_edge(this->root.as_node_ref()), 0,               this->GetSize()}; }
		Iter end()   { return {BTree:: last_leaf_edge(this->root.as_node_ref()), this->GetSize(), this->GetSize()}; }
		class Keys   Keys()   { return {*this}; }
		class Values Values() { return {*this}; }

	public:
		void swap(BTreeMap& other) { std::swap(this->root, other.root); std::swap(this->len, other.len); }

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
		Optional<V&> get(Key&& key) {
			auto entry = this->Entry(std::forward<Key>(key));
			if (!entry.IsVacant()) {
				return {entry.OccupiedGet()};
			} else {
				return {};
			}
		}

	private:
		class Entry	{
		public:
			static Entry Vacant  (KVERef kv_ref, size_t& len, const K&  key) { return Entry(kv_ref, len, key); }
			static Entry Vacant  (KVERef kv_ref, size_t& len,       K&& key) { return Entry(kv_ref, len, std::move(key)); }
			static Entry Occupied(KVERef kv_ref, size_t& len)                { return Entry(kv_ref, len); }

		public:
			bool IsVacant() { return static_cast<bool>(key); }

			V& OrInsert(const V&  value) { return this->or_insert(value); }
			V& OrInsert(      V&& value) { return this->or_insert(std::move(value)); }

			V& VacantInsert(const V&  value) { return this->vacant_insert(value); }
			V& VacantInsert(      V&& value) { return this->vacant_insert(std::move(value)); }

			V OccupiedInsert(const V&  value) { return this->occupied_insert(value); }
			V OccupiedInsert(      V&& value) { return this->occupied_insert(std::move(value)); }

			V& OccupiedGet() { ASSERTE(!this->IsVacant(), "Cannot read value from a vacant map entry!"); return this->kv_ref.node_ref.node->values[this->kv_ref.idx]; }

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

		struct KV {
			const K& key;
			V& value;
			const KV* operator->() const { return this; } //note(vuko): needed by the iterator's own operator ->, since it returns KV by value
		};

		class Iter : public std::iterator<std::bidirectional_iterator_tag, KV> {
		public:
			Iter(KVERef kv_ref, size_t position, size_t map_len) : current(kv_ref), next_forward(kv_ref), next_backward(kv_ref), position(position), map_len(map_len) {
					if (this->position != this->map_len) {
						if (kv_ref.idx < kv_ref.node_ref.node->len) {
							this->next_forward = KVERef{kv_ref.node_ref, kv_ref.idx + 1};
						}
					}
				}
		public:
			bool operator==(const Iter& other) const { return this->current == other.current; }
			bool operator!=(const Iter& other) const { return !(*this == other); }
		public:
			KV operator*()  const { return {this->current.node_ref.node->keys[this->current.idx], this->current.node_ref.node->values[this->current.idx]}; }
			KV operator->() const { return operator*(); }
		public:
			Iter& operator++() { this->next_backward = this->current; this->current = this->next(); return *this; }
			Iter& operator--() { this->next_forward  = this->current; this->current = this->prev(); return *this; }
			Iter operator++(int) { Iter ret(this->current, this->position, this->map_len); operator++(); return ret; }
			Iter operator--(int) { Iter ret(this->current, this->position, this->map_len); operator--(); return ret; }
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
		private:
			KVERef current;
			KVERef next_forward;
			KVERef next_backward;
			size_t position;
			size_t map_len;
		};

		class Keys {
			class KIter; //note(vuko): GCC is being weird here, so we can't name the class simply Iter and also additionally we have to forward-declare it
		public:
			Keys(BTreeMap& map) : map(map) {}
		public:
			KIter begin() { return {this->map.begin()}; }
			KIter end() { return {this->map.end()}; }
		private:
			using ParentIter = typename BTreeMap::Iter;
			class KIter : public std::iterator<std::bidirectional_iterator_tag, K> {
			public:
				KIter(ParentIter iter) : iter(iter) {}
			public:
				bool operator==(const KIter& other) const { return this->iter == other.iter; }
				bool operator!=(const KIter& other) const { return !(*this == other); }
			public:
				const K& operator*()  const { return this->iter->key; }
				const K& operator->() const { return operator*(); }
			public:
				KIter& operator++() { ++(this->iter); return *this; }
				KIter operator++(int) { KIter ret(this->iter); operator++(); return ret; }
				KIter& operator--() { --(this->iter); return *this; }
				KIter operator--(int) { KIter ret(this->iter); operator--(); return ret; }
			private:
				ParentIter iter;
			};
		private:
			BTreeMap& map;
		};

		class Values {
			class VIter;
		public:
			Values(BTreeMap& map) : map(map) {}
		public:
			VIter begin() { return {this->map.begin()}; }
			VIter end() { return {this->map.end()}; }
		private:
			using ParentIter = typename BTreeMap::Iter;
			class VIter : public std::iterator<std::bidirectional_iterator_tag, V> {
			public:
				VIter(ParentIter iter) : iter(iter) {}
			public:
				bool operator==(const VIter& other) const { return this->iter == other.iter; }
				bool operator!=(const VIter& other) const { return !(*this == other); }
			public:
				K& operator*()  const { return this->iter->value; }
				K& operator->() const { return operator*(); }
			public:
				VIter& operator++() { ++(this->iter); return *this; }
				VIter operator++(int) { VIter ret(this->iter); operator++(); return ret; }
				VIter& operator--() { --(this->iter); return *this; }
				VIter operator--(int) { VIter ret(this->iter); operator--(); return ret; }
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
