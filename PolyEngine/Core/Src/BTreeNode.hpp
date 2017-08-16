#pragma once

#include "Defines.hpp"
#include "UnsafeStorage.hpp"

namespace Poly {
	namespace Impl {

		//note(vuko): used to move into uninitialized memory, when using operator=() and (by extension) std::move([range]) is not possible
		template<typename InIt, typename OutIt>
		auto constructing_move(InIt first, InIt last, OutIt dest_first) {
			using InType  = typename std::iterator_traits<InIt>::value_type;
			using OutType = typename std::iterator_traits<OutIt>::value_type;

			for (; first < last; ++first, ++dest_first) {
				::new(&*(dest_first)) OutType(std::move(*first));
				first->~InType();
			}

			return dest_first;
		};

		template<typename InputIter, typename DestIter>
		auto constructing_move_backwards(InputIter first, InputIter last, DestIter dest_last) {
			using InType  = typename std::iterator_traits<InputIter>::value_type;
			using OutType = typename std::iterator_traits<DestIter>::value_type;

			while (first < last) {
				::new(&*(--dest_last)) OutType(std::move(*(--last)));
				last->~InType();
			}

			return dest_last;
		};

		template<typename K, typename V, size_t B>
		struct BTree {
			constexpr static size_t CAPACITY = 2 * B - 1u;
			constexpr static size_t MIN_LEN = B - 1u;
			static_assert(CAPACITY >= 3, "Capacity must be at least 3 or greater!");

			struct BranchNode;
			struct LeafNode {
				LeafNode() : parent(nullptr), len(0) {}
				~LeafNode() {
					//since the backing storage has no idea about how many values it contains, we need to destroy them explicitly
					keys  .destruct(len);
					values.destruct(len);
				}
				auto as_branch() { return static_cast<BranchNode*>(this); };

				UnsafeStorage<K, CAPACITY> keys;
				UnsafeStorage<V, CAPACITY> values;
				BranchNode* parent;
				uint16_t edge_idx; //todo(vuko): rename? choose: edge_idx_in_parent, parent_idx, position
				uint16_t len;
			};

			struct BranchNode : public LeafNode {
				BranchNode() : LeafNode() {}
				UnsafeStorage<LeafNode*, CAPACITY + 1u> edges; //len+1 initialized and valid
			};

			struct NodeRef;
			struct KVERef;

			struct Root {
				LeafNode* node;
				size_t height;

				NodeRef as_node_ref() { return {this->height, this->node, this}; };

				NodeRef push_level() {
					const auto new_node = new BranchNode();
					new_node->edges[0] = node;

					node = new_node;
					height += 1u;

					const auto node_ref = this->as_node_ref();
					KVERef{node_ref, 0}.correct_parent_link();
					return node_ref;
				};

				void pop_level() {
					ASSERTE(this->height > 0, "No levels to pop!");

					LeafNode* const top = node;
					node = KVERef{this->as_node_ref(), 0}.descend().node;
					node->parent = nullptr;
					height -= 1u;

					delete top->as_branch();
				}
			};

			using Edge = Root; //mostly the same

			struct NodeRef {
				size_t height;
				LeafNode* node;
				Root* root;

				bool operator==(const NodeRef& other) const { return this->height == other.height && this->node == other.node && this->root == other.root; }

				//todo(vuko): these accessors or maybe an overloaded operator-> might make the code more readable
				//auto& len() { return node->len; }
				//auto& keys() { return node->keys; }
				//auto& values() { return node->values; }
				//auto& edges() { ASSERTE(height, "Attempting to use leaf as a branch!"); return node->as_branch()->edges; }

				struct AscensionResult {
					AscensionResult(KVERef edge_ref) : succeeded(true), parent(edge_ref) {}
					AscensionResult(NodeRef self)    : succeeded(false), self(self) {}
					bool succeeded;
					union {
						KVERef parent; //valid if succeeded
						NodeRef self; //valid if not succeeded; note(vuko): technically this is redundant, but actually simplifies things a little bit
					};
				};
				AscensionResult const ascend() {
					if (node->parent) {
						return {KVERef{ NodeRef{ height + 1u, node->parent, root }, node->edge_idx} };
					} else {
						return {*this};
					}
				};

				template<typename Key, typename Val>
				void push_back(Key&& key, Val&& value) {
					//leaf
					ASSERTE(node->len < CAPACITY, "No space left in the leaf!");

					const uint16_t idx = node->len;

					::new(&node->keys  [idx]) K(std::forward<Key>(key));
					::new(&node->values[idx]) V(std::forward<Val>(value));

					node->len += 1u;
				}

				template<typename Key, typename Val>
				void push_front(Key&& key, Val&& value) {
					//leaf
					ASSERTE(node->len < CAPACITY, "No space left in the leaf!");

					constructing_move_backwards(node->keys.begin(),   node->keys.begin()   + node->len, node->keys.begin()   + node->len + 1u);
					::new(&node->keys  [0]) K(std::forward<Key>(key));

					constructing_move_backwards(node->values.begin(), node->values.begin() + node->len, node->values.begin() + node->len + 1u);
					::new(&node->values[0]) V(std::forward<Val>(value));

					node->len += 1u;
				}

				template<typename Key, typename Val>
				void push_back(Key&& key, Val&& value, const Edge edge) {
					//branch
					ASSERTE(edge.height == height - 1u, "Attempting to push an edge node of incorrect height!");
					ASSERTE(node->len < CAPACITY, "No space left in the branch!");

					const size_t idx = node->len;

					::new(&node->keys  [idx]) K(std::forward<Key>(key));
					::new(&node->values[idx]) V(std::forward<Val>(value));
					node->as_branch()->edges[idx + 1u] = edge.node;

					node->len += 1u;

					KVERef{*this, idx + 1u}.correct_parent_link();
				}

				template<typename Key, typename Val>
				void push_front(Key&& key, Val&& value, const Edge edge) {
					//branch
					ASSERTE(edge.height == height - 1u, "Attempting to push an edge node of incorrect height!");
					ASSERTE(node->len < CAPACITY, "No space left in the branch");

					constructing_move_backwards(node->keys.begin(),   node->keys.begin()   + node->len, node->keys.begin()   + node->len + 1u);
					::new(&node->keys  [0]) K(std::forward<Key>(key));

					constructing_move_backwards(node->values.begin(), node->values.begin() + node->len, node->values.begin() + node->len + 1u);
					::new(&node->values[0]) V(std::forward<Val>(value));

					auto& edges = node->as_branch()->edges;
					std::move_backward(edges.begin(), edges.begin() + node->len + 1u, edges.begin() + node->len + 2u);
					edges[0] = edge.node;

					node->len += 1u;

					for (size_t i = 0; i < node->len + 1u; i++) {
						KVERef{*this, i}.correct_parent_link();
					}
				}

				struct PopResult {
					K key;
					V value;
					Edge edge; //optional
				};

				PopResult pop_back() {
					ASSERTE(node->len > 0, "No entry to pop in the node!");

					const uint16_t idx = node->len - 1u;

					PopResult ret{std::move(node->keys[idx]), std::move(node->values[idx]), Edge{}};
					node->keys  .destruct_at(idx);
					node->values.destruct_at(idx);

					if (height) {
						//branch
						Edge new_edge = {node->as_branch()->edges[idx + 1u], height - 1u};
						new_edge.as_node_ref().node->parent = nullptr;
						ret.edge = new_edge;
					}

					node->len -= 1u;
					return ret;
				}

				PopResult pop_front() {
					ASSERTE(node->len > 0, "No entry to pop in the node!");

					auto& len    = node->len;
					auto& keys   = node->keys;
					auto& values = node->values;

					PopResult ret{std::move(keys[0]), std::move(values[0]), Edge{}};
					std::move(keys.begin()   + 1u, keys.begin()   + len, keys.begin());
					std::move(values.begin() + 1u, values.begin() + len, values.begin());
					keys  .destruct_at(len - 1u);
					values.destruct_at(len - 1u);

					if (height) {
						//branch
						auto& edges = node->as_branch()->edges;

						LeafNode* const child = edges[0];
						std::move(edges.begin() + 1u, edges.begin() + len + 1u, edges.begin());

						Edge new_edge = {child, height - 1u};
						new_edge.as_node_ref().node->parent = nullptr;
						ret.edge = new_edge;

						for (size_t i = 0; i < len; ++i) {
							KVERef{*this, i}.correct_parent_link();
						}
					}

					len -= 1u;
					return ret;
				}
			};

			struct KVERef { //todo(vuko): does double duty as KeyValueRef/EdgeRef; split?
				NodeRef node_ref;
				size_t idx;

				bool operator==(const KVERef& other) const { return node_ref == other.node_ref && idx == other.idx; }

				//todo(vuko): these accessors or maybe an overloaded operator-> might make the code more readable
				//size_t& height() { return node_ref.height; }
				//LeafNode<K, V>* node() { return node_ref.node; }
				//Root<K, V>* root() { return node_ref.root; }
				//size_t& len() { return node_ref.len(); }
				//auto& keys() { return node_ref.keys(); }
				//auto& values() { return node_ref.values(); }
				//auto& edges() { return node_ref.edges(); }
				NodeRef descend() const { ASSERTE(node_ref.height, "Cannot descend from a leaf node!"); return {node_ref.height - 1u, node_ref.node->as_branch()->edges[idx], node_ref.root}; }; //mark: edge

				struct SplitResult {
					NodeRef old_node_left; //old node truncated to only contain key/value pairs (and possibly edges) to the left of the handle
					K key;   //pointed to by the handle, extracted
					V value; //pointed to by the handle, extracted
					Edge new_edge_right; //new node containing key/value pairs (and possibly edges) from the right of the handle
				};

				SplitResult split_leaf() { //mark: kv
					auto new_node = new LeafNode();
					auto old_node = node_ref.node;

					const auto k = std::move(old_node->keys  [idx]);
					const auto v = std::move(old_node->values[idx]);
					old_node->keys  .destruct_at(idx);
					old_node->values.destruct_at(idx);

					constructing_move(old_node->keys.begin()   + idx + 1u, old_node->keys.begin()   + old_node->len, new_node->keys.begin());
					constructing_move(old_node->values.begin() + idx + 1u, old_node->values.begin() + old_node->len, new_node->values.begin());

					const auto new_len = old_node->len - idx - 1u;
					old_node->len = static_cast<uint16_t>(idx);
					new_node->len = static_cast<uint16_t>(new_len);

					return SplitResult{node_ref, std::move(k), std::move(v), Edge{new_node, 0}};
				}

				SplitResult split_branch() { //mark: kv
					auto new_node = new BranchNode();
					auto old_node = node_ref.node;

					const auto k = std::move(old_node->keys  [idx]);
					const auto v = std::move(old_node->values[idx]);
					old_node->keys  .destruct_at(idx);
					old_node->values.destruct_at(idx);

					constructing_move(old_node->keys.begin()   + idx + 1u, old_node->keys.begin()   + old_node->len, new_node->keys.begin());
					constructing_move(old_node->values.begin() + idx + 1u, old_node->values.begin() + old_node->len, new_node->values.begin());
					std::move(old_node->as_branch()->edges.begin() + idx + 1u, old_node->as_branch()->edges.begin() + old_node->len + 1u, new_node->edges.begin());

					const auto new_len = old_node->len - idx - 1u;
					old_node->len = static_cast<uint16_t>(idx);
					new_node->len = static_cast<uint16_t>(new_len);

					Edge new_edge = {new_node, node_ref.height};

					for(size_t i = 0; i < new_node->len + 1u; i++) {
						KVERef{new_edge.as_node_ref(), i}.correct_parent_link();
					}

					return SplitResult{node_ref, std::move(k), std::move(v), new_edge};
				}

				struct InsertResult {
					InsertResult(KVERef handle,            V* value_ref) : fit(true ), handle(handle),                        value_ref(value_ref) {}
					InsertResult(SplitResult split_result, V* value_ref) : fit(false), split_result(std::move(split_result)), value_ref(value_ref) {}
					InsertResult(const InsertResult&  other) : fit(other.fit), value_ref(other.value_ref) {
						if (other.fit) {
							this->handle = other.handle;
						} else {
							::new(&this->split_result) SplitResult(other.split_result);
						}
					}
					InsertResult(      InsertResult&& other) : fit(other.fit), value_ref(other.value_ref) {
						if (other.fit) {
							this->handle = other.handle;
						} else {
							::new(&this->split_result) SplitResult(std::move(other.split_result));
						}
					}
					~InsertResult() { if (!this->fit) { split_result.~SplitResult(); } };
					InsertResult& operator=(const InsertResult& other) {
						if (!this->fit) {
							this->split_result.~SplitResult();
						}

						if (other.fit) {
							this->handle = other.handle;
						} else {
							::new(&this->split_result) SplitResult(other.split_result);
						}

						this->fit = other.fit;
						this->value_ref = other.value_ref;

						return *this;
					}
					InsertResult& operator=(InsertResult&& other) {
						if (!this->fit) {
							this->split_result.~SplitResult();
						}

						if (other.fit) {
							this->handle = other.handle;
						} else {
							::new(&this->split_result) SplitResult(std::move(other.split_result));
						}

						this->fit = other.fit;
						this->value_ref = other.value_ref;

						return *this;
					}

					bool fit;
					union {
						KVERef handle;            //valid if fit
						SplitResult split_result; //valid if did not fit
					};
					V* value_ref; //non-owning
				};

				template<typename Key, typename Val>
				V* leaf_insert_fit(Key&& key, Val&& value) { //mark: edge
					auto& node_len = node_ref.node->len;
					ASSERTE(node_len < CAPACITY, "No space left in the leaf!");

					auto& keys = node_ref.node->keys;
					constructing_move_backwards(keys.begin()   + idx, keys.begin()   + node_len, keys.begin()   + node_len + 1u);
					::new(&keys  [idx]) K(std::forward<Key>(key));

					auto& values = node_ref.node->values;
					constructing_move_backwards(values.begin() + idx, values.begin() + node_len, values.begin() + node_len + 1u);
					::new(&values[idx]) V(std::forward<Val>(value));

					node_len += 1u;
					return &values[idx];
				}

				template<typename Key, typename Val>
				InsertResult leaf_insert(Key&& key, Val&& value) { //mark: edge
					if (node_ref.node->len < CAPACITY) {
						V* const v = leaf_insert_fit(std::forward<Key>(key), std::forward<Val>(value));

						return {KVERef{node_ref, idx}, v};
					} else {
						KVERef middle = {node_ref, B};
						auto split_result = middle.split_leaf();

						NodeRef left = split_result.old_node_left;
						NodeRef right = split_result.new_edge_right.as_node_ref();

						V* const v = (idx <= B) ?
						             KVERef{left,  idx           }.leaf_insert_fit(std::forward<Key>(key), std::forward<Val>(value))
						                        :
						             KVERef{right, idx - (B + 1u)}.leaf_insert_fit(std::forward<Key>(key), std::forward<Val>(value));

						return {std::move(split_result), v};
					}
				}

				template<typename Key, typename Val>
				void branch_insert_fit(Key&& key, Val&& value, const Edge edge) { //mark: edge
					uint16_t& node_len = node_ref.node->len;
					ASSERTE(node_len < CAPACITY, "No space left in the branch!");
					ASSERTE(edge.height == node_ref.height - 1u, "Attempting to insert an edge node of incorrect height. Tree corrupted!");

					auto& keys = node_ref.node->keys;
					constructing_move_backwards(keys.begin()   + idx, keys.begin()   + node_len, keys.begin()   + node_len + 1u);
					::new(&keys  [idx]) K(std::forward<Key>(key));

					auto& values = node_ref.node->values;
					constructing_move_backwards(values.begin() + idx, values.begin() + node_len, values.begin() + node_len + 1u);
					::new(&values[idx]) V(std::forward<Val>(value));

					auto& edges = node_ref.node->as_branch()->edges;
					std::move_backward(edges.begin() + idx + 1u, edges.begin() + node_len + 1u, edges.begin() + node_len + 2u);
					edges[idx + 1u] = edge.node;

					node_len += 1u;

					for (size_t i = idx + 1u; i < node_len + 1u; i++) {
						KVERef{node_ref, i}.correct_parent_link();
					}
				}

				template<typename Key, typename Val>
				InsertResult branch_insert(Key&& key, Val&& value, const Edge edge) { //mark: edge
					ASSERTE(edge.height == node_ref.height - 1u, "Attempting to insert an edge node of incorrect height. Tree corrupted!");

					if (node_ref.node->len < CAPACITY) {
						branch_insert_fit(std::forward<Key>(key), std::forward<Val>(value), edge);

						return {KVERef{node_ref, idx}, nullptr}; //note(vuko): returned value reference is never used and so can be safely null
					} else {
						KVERef middle = {node_ref, B};
						auto split_result = middle.split_branch();

						NodeRef left = split_result.old_node_left;
						NodeRef right = split_result.new_edge_right.as_node_ref();

						if (idx <= B) {
							KVERef{left,  idx           }.branch_insert_fit(std::forward<Key>(key), std::forward<Val>(value), edge);
						} else {
							KVERef{right, idx - (B + 1u)}.branch_insert_fit(std::forward<Key>(key), std::forward<Val>(value), edge);
						}

						return InsertResult(std::move(split_result), nullptr);
					}
				}

				struct RemoveResult {
					K k;
					V v;
				};
				RemoveResult leaf_remove() { //mark: kv
					auto& keys     = node_ref.node->keys;
					auto& values   = node_ref.node->values;
					auto& node_len = node_ref.node->len;

					RemoveResult ret{std::move(keys[idx]), std::move(values[idx])};

					std::move(keys.begin()   + idx + 1u, keys.begin()   + node_len, keys.begin()   + idx);
					std::move(values.begin() + idx + 1u, values.begin() + node_len, values.begin() + idx);
					keys  .destruct_at(node_len - 1u);
					values.destruct_at(node_len - 1u);

					node_len -= 1u;

					return ret;
				};

				bool can_merge() const { return KVERef{node_ref, idx}.descend().node->len + KVERef{node_ref, idx + 1u}.descend().node->len + 1u <= CAPACITY; } //mark: kv

				KVERef merge() { //mark: kv
					NodeRef left_node = KVERef{node_ref, idx}.descend();
					uint16_t& left_len = left_node.node->len;
					uint16_t old_left_len = left_len;

					NodeRef right_node = KVERef{node_ref, idx + 1u}.descend();
					uint16_t& right_len = right_node.node->len;
					uint16_t old_right_len = right_len;

					ASSERTE(left_len + 1u + right_len <= CAPACITY, "Nodes are too big to merge!");

					auto& len = node_ref.node->len;

					auto& keys       = node_ref  .node->keys;
					auto& left_keys  = left_node .node->keys;
					auto& right_keys = right_node.node->keys;

					::new(&left_keys  [left_len]) K(std::move(keys[idx]));
					std::move(keys.begin()   + idx + 1u, keys.begin()   + len, keys.begin()   + idx);
					keys.destruct_at(len - 1u);
					constructing_move(right_keys.begin(),   right_keys.begin()   + right_len, left_keys.begin()   + left_len + 1u);

					auto& values       = node_ref  .node->values;
					auto& left_values  = left_node .node->values;
					auto& right_values = right_node.node->values;

					::new(&left_values[left_len]) V(std::move(values[idx]));
					std::move(values.begin() + idx + 1u, values.begin() + len, values.begin() + idx);
					values.destruct_at(len - 1u);
					constructing_move(right_values.begin(), right_values.begin() + right_len, left_values.begin() + left_len + 1u);

					auto& edges = node_ref.node->as_branch()->edges;
					std::move(edges.begin() + idx + 2u, edges.begin() + len + 1u, edges.begin() + idx + 1u);
					for (size_t i = idx + 1u; i < len; i++) {
						KVERef{node_ref, i}.correct_parent_link();
					}

					len -= 1u;
					left_len += 1u + right_len;
					right_len = 0; //we just moved out (and destroyed) all it holds

					if (node_ref.height > 1) {
						auto& right_edges = right_node.node->as_branch()->edges;
						auto& left_edges  = left_node .node->as_branch()->edges;
						std::move(right_edges.begin(), right_edges.begin() + old_right_len + 1u, left_edges.begin() + old_left_len + 1u);

						for (size_t i = old_left_len + 1u; i < old_left_len + old_right_len + 2u; i++) {
							KVERef{left_node, i}.correct_parent_link();
						}

						delete right_node.node->as_branch();
					} else {
						delete right_node.node;
					}

					return *this;
				};

				void steal_left() { //mark: kv
					auto pop_result = KVERef{node_ref, idx}.descend().pop_back();

					std::swap(node_ref.node->keys  [idx], pop_result.key);
					std::swap(node_ref.node->values[idx], pop_result.value);

					NodeRef child = KVERef{node_ref, idx + 1u}.descend();
					if (child.height == 0) {
						child.push_front(pop_result.key, pop_result.value);
					} else {
						child.push_front(pop_result.key, pop_result.value, pop_result.edge);
					}
				}

				void steal_right() { //mark: kv
					auto pop_result = KVERef{node_ref, idx + 1u}.descend().pop_front();

					std::swap(node_ref.node->keys  [idx], pop_result.key);
					std::swap(node_ref.node->values[idx], pop_result.value);

					NodeRef child = KVERef{node_ref, idx}.descend();
					if (child.height == 0) {
						child.push_back(pop_result.key, pop_result.value);
					} else {
						child.push_back(pop_result.key, pop_result.value, pop_result.edge);
					}
				}

				void correct_parent_link() { //mark: edge
					NodeRef child = descend();
					child.node->parent = this->node_ref.node->as_branch();
					child.node->edge_idx = static_cast<uint16_t>(this->idx);
				}
			};

			static KVERef first_leaf_edge(NodeRef node_ref) {
				while (node_ref.height > 0) {
					node_ref = KVERef{node_ref, 0}.descend();
				}
				return KVERef{node_ref, 0};
			};

			static KVERef last_leaf_edge(NodeRef node_ref) {
				while (node_ref.height > 0) {
					node_ref = KVERef{node_ref, node_ref.node->len}.descend();
				}
				return KVERef{node_ref, node_ref.node->len};
			};

		};

	} //namespace Impl
} //namespace Poly
