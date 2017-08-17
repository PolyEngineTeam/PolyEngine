#pragma once

#include "Defines.hpp"
#include "UnsafeStorage.hpp"

namespace Poly
{
	namespace Impl
	{
		namespace Object = ObjectLifetimeHelper;

		//note(vuko): used to move into uninitialized memory, when using operator=() and (by extension) std::move([range]) is not possible
		template<typename InIt, typename OutIt>
		auto ConstructingMove(InIt first, InIt last, OutIt destFirst)
		{
			for (; first < last; first++, destFirst++)
			{
				Object::MoveCreate(&*destFirst, std::move(*first));
				Object::Destroy(&*first);
			}

			return destFirst;
		};

		template<typename InputIter, typename DestIter>
		auto ConstructingMoveBackwards(InputIter first, InputIter last, DestIter destLast)
		{
			while (first < last)
			{
				Object::MoveCreate(&*(--destLast), std::move(*(--last)));
				Object::Destroy(&*last);
			}

			return destLast;
		};

		template<typename K, typename V, size_t B>
		struct BTree : public BaseObject<> //Ordnung muss sein xDDD
		{
			constexpr static size_t CAPACITY = 2 * B - 1u;
			constexpr static size_t MIN_LEN  = B - 1u;
			static_assert(CAPACITY >= 3, "Capacity must be at least 3 or greater!");

			struct BranchNode;
			struct LeafNode : public BaseObjectLiteralType<>
			{
				LeafNode() : parent(nullptr), len(0) {}
				~LeafNode()
				{
					//since the backing storage has no idea about how many elements it contains, we need to destroy them explicitly
					keys.Destruct(len);
					values.Destruct(len);
				}
				auto AsBranch() { return static_cast<BranchNode*>(this); };

				UnsafeStorage<K, CAPACITY> keys;
				UnsafeStorage<V, CAPACITY> values;
				BranchNode* parent;
				uint16_t position; //position (edge index) in parent
				uint16_t len;
			};

			struct BranchNode : public LeafNode //note(vuko): not a true is-a relation, but simplifies the code
			{
				BranchNode() : LeafNode() {}
				UnsafeStorage<LeafNode*, CAPACITY + 1u> edges; //len+1 initialized and valid
			};

			struct NodeRef;
			struct KVERef;

			struct Root
			{
				LeafNode* node;
				size_t height;

				NodeRef AsNodeRef() { return {this->height, this->node, this}; };

				NodeRef PushLevel()
				{
					const auto newNode = new BranchNode();
					newNode->edges[0]  = node;

					node = newNode;
					height += 1u;

					const auto nodeRef = this->AsNodeRef();
					KVERef{nodeRef, 0}.CorrectParentLink();
					return nodeRef;
				};

				void PopLevel()
				{
					ASSERTE(this->height > 0, "No levels to pop!");

					LeafNode* const top = node;
					node = KVERef{this->AsNodeRef(), 0}.Descend().node;
					node->parent = nullptr;
					height -= 1u;

					delete top->AsBranch();
				}
			};

			using Edge = Root; //mostly the same

			struct NodeRef
			{
				size_t height;
				LeafNode* node;
				Root* root;

				bool operator==(const NodeRef& other) const { return this->height == other.height && this->node == other.node && this->root == other.root; }

				//todo(vuko): these accessors or maybe an overloaded operator-> might make the code more readable
				//auto& len() { return node->len; }
				//auto& keys() { return node->keys; }
				//auto& values() { return node->values; }
				//auto& edges() { ASSERTE(height, "Attempting to use leaf as a branch!"); return node->AsBranch()->edges; }

				struct AscensionResult : public BaseObjectLiteralType<>
				{
					AscensionResult(KVERef edgeRef) : succeeded(true), parent(edgeRef) {}
					AscensionResult(NodeRef self)   : succeeded(false), self(self) {}
					bool succeeded;
					union
					{
						KVERef parent; //valid if succeeded
						NodeRef self;  //valid if not succeeded; note(vuko): technically this is redundant, but actually simplifies things a little bit
					};
				};

				AscensionResult const Ascend()
				{
					if (node->parent)
					{
						return {KVERef{ NodeRef{ height + 1u, node->parent, root }, node->position} };
					}
					else
					{
						return {*this};
					}
				};

				template<typename Key, typename Val>
				void PushBack(Key&& key, Val&& value)
				{
					//leaf
					ASSERTE(node->len < CAPACITY, "No space left in the leaf!");

					const uint16_t idx = node->len;

					::new(&node->keys  [idx]) K(std::forward<Key>(key)); //todo(muniu): need a ObjectLifetimeHelpers::ForwardingConstruct
					::new(&node->values[idx]) V(std::forward<Val>(value));

					node->len += 1u;
				}

				template<typename Key, typename Val>
				void PushFront(Key&& key, Val&& value)
				{
					//leaf
					ASSERTE(node->len < CAPACITY, "No space left in the leaf!");

					ConstructingMoveBackwards(node->keys.begin(),   node->keys.begin()   + node->len, node->keys.begin()   + node->len + 1u);
					::new(&node->keys  [0]) K(std::forward<Key>(key));

					ConstructingMoveBackwards(node->values.begin(), node->values.begin() + node->len, node->values.begin() + node->len + 1u);
					::new(&node->values[0]) V(std::forward<Val>(value));

					node->len += 1u;
				}

				template<typename Key, typename Val>
				void PushBack(Key&& key, Val&& value, const Edge edge)
				{
					//branch
					ASSERTE(edge.height == height - 1u, "Attempting to push an edge node of incorrect height!");
					ASSERTE(node->len < CAPACITY, "No space left in the branch!");

					const size_t idx = node->len;

					::new(&node->keys  [idx]) K(std::forward<Key>(key));
					::new(&node->values[idx]) V(std::forward<Val>(value));
					node->AsBranch()->edges[idx + 1u] = edge.node;

					node->len += 1u;

					KVERef{*this, idx + 1u}.CorrectParentLink();
				}

				template<typename Key, typename Val>
				void PushFront(Key&& key, Val&& value, const Edge edge)
				{
					//branch
					ASSERTE(edge.height == height - 1u, "Attempting to push an edge node of incorrect height!");
					ASSERTE(node->len < CAPACITY, "No space left in the branch");

					ConstructingMoveBackwards(node->keys.begin(),   node->keys.begin()   + node->len, node->keys.begin()   + node->len + 1u);
					::new(&node->keys  [0]) K(std::forward<Key>(key));

					ConstructingMoveBackwards(node->values.begin(), node->values.begin() + node->len, node->values.begin() + node->len + 1u);
					::new(&node->values[0]) V(std::forward<Val>(value));

					auto& edges = node->AsBranch()->edges;
					std::move_backward(edges.begin(), edges.begin() + node->len + 1u, edges.begin() + node->len + 2u);
					edges[0] = edge.node;

					node->len += 1u;

					for (size_t i = 0; i < node->len + 1u; i++)
					{
						KVERef{*this, i}.CorrectParentLink();
					}
				}

				struct PopResult
				{
					K key;
					V value;
					Edge edge; //optional
				};

				PopResult PopBack()
				{
					ASSERTE(node->len > 0, "No entry to pop in the node!");

					const uint16_t idx = node->len - 1u;

					PopResult ret{std::move(node->keys[idx]), std::move(node->values[idx]), Edge{}};
					node->keys  .DestructAt(idx);
					node->values.DestructAt(idx);

					if (height)
					{
						//branch
						Edge newEdge = {node->AsBranch()->edges[idx + 1u], height - 1u};
						newEdge.AsNodeRef().node->parent = nullptr;
						ret.edge = newEdge;
					}

					node->len -= 1u;
					return ret;
				}

				PopResult PopFront()
				{
					ASSERTE(node->len > 0, "No entry to pop in the node!");

					auto& len    = node->len;
					auto& keys   = node->keys;
					auto& values = node->values;

					PopResult ret{std::move(keys[0]), std::move(values[0]), Edge{}};
					std::move(keys.begin()   + 1u, keys.begin()   + len, keys.begin());
					std::move(values.begin() + 1u, values.begin() + len, values.begin());
					keys  .DestructAt(len - 1u);
					values.DestructAt(len - 1u);

					if (height)
					{
						//branch
						auto& edges = node->AsBranch()->edges;

						LeafNode* const child = edges[0];
						std::move(edges.begin() + 1u, edges.begin() + len + 1u, edges.begin());

						Edge new_edge = {child, height - 1u};
						new_edge.AsNodeRef().node->parent = nullptr;
						ret.edge = new_edge;

						for (size_t i = 0; i < len; i++)
						{
							KVERef{*this, i}.CorrectParentLink();
						}
					}

					len -= 1u;
					return ret;
				}
			};

			struct KVERef //todo(vuko): does double duty as KeyValueRef/EdgeRef; split?
			{
				NodeRef nodeRef;
				size_t idx;

				bool operator==(const KVERef& other) const { return nodeRef == other.nodeRef && idx == other.idx; }

				//todo(vuko): these accessors or maybe an overloaded operator-> might make the code more readable
				//size_t& height() { return nodeRef.height; }
				//LeafNode<K, V>* node() { return nodeRef.node; }
				//Root<K, V>* root() { return nodeRef.root; }
				//size_t& len() { return nodeRef.len(); }
				//auto& keys() { return nodeRef.keys(); }
				//auto& values() { return nodeRef.values(); }
				//auto& edges() { return nodeRef.edges(); }
				NodeRef Descend() const { ASSERTE(nodeRef.height, "Cannot Descend from a leaf node!"); return {nodeRef.height - 1u, nodeRef.node->AsBranch()->edges[idx], nodeRef.root}; }; //mark: edge

				struct SplitResult
				{
					NodeRef oldNodeLeft; //old node truncated to only contain key/value pairs (and possibly edges) to the left of the handle
					K key;               //pointed to by the handle, extracted
					V value;             //pointed to by the handle, extracted
					Edge newEdgeRight;   //new node containing key/value pairs (and possibly edges) from the right of the handle
				};

				SplitResult SplitLeaf() //mark: kv
				{
					auto newNode = new LeafNode();
					auto oldNode = nodeRef.node;

					const auto k = std::move(oldNode->keys  [idx]);
					const auto v = std::move(oldNode->values[idx]);
					oldNode->keys  .DestructAt(idx);
					oldNode->values.DestructAt(idx);

					ConstructingMove(oldNode->keys.begin()   + idx + 1u, oldNode->keys.begin()   + oldNode->len, newNode->keys.begin());
					ConstructingMove(oldNode->values.begin() + idx + 1u, oldNode->values.begin() + oldNode->len, newNode->values.begin());

					const auto new_len = oldNode->len - idx - 1u;
					oldNode->len = static_cast<uint16_t>(idx);
					newNode->len = static_cast<uint16_t>(new_len);

					return SplitResult{nodeRef, std::move(k), std::move(v), Edge{newNode, 0}};
				}

				SplitResult SplitBranch() //mark: kv
				{
					auto newNode = new BranchNode();
					auto oldNode = nodeRef.node;

					const auto k = std::move(oldNode->keys  [idx]);
					const auto v = std::move(oldNode->values[idx]);
					oldNode->keys  .DestructAt(idx);
					oldNode->values.DestructAt(idx);

					ConstructingMove(oldNode->keys.begin()   + idx + 1u, oldNode->keys.begin()   + oldNode->len, newNode->keys.begin());
					ConstructingMove(oldNode->values.begin() + idx + 1u, oldNode->values.begin() + oldNode->len, newNode->values.begin());
					std::move(oldNode->AsBranch()->edges.begin() + idx + 1u, oldNode->AsBranch()->edges.begin() + oldNode->len + 1u, newNode->edges.begin());

					const auto newLen = oldNode->len - idx - 1u;
					oldNode->len = static_cast<uint16_t>(idx);
					newNode->len = static_cast<uint16_t>(newLen);

					Edge newEdge = {newNode, nodeRef.height};

					for(size_t i = 0; i < newNode->len + 1u; i++)
					{
						KVERef{newEdge.AsNodeRef(), i}.CorrectParentLink();
					}

					return SplitResult{nodeRef, std::move(k), std::move(v), newEdge};
				}

				struct InsertResult : public BaseObjectLiteralType<>
				{
					InsertResult(KVERef handle,           V* valueRef) : fit(true ), handle(handle),                      valueRef(valueRef) {}
					InsertResult(SplitResult splitResult, V* valueRef) : fit(false), splitResult(std::move(splitResult)), valueRef(valueRef) {}
					InsertResult(const InsertResult& other) : fit(other.fit), valueRef(other.valueRef)
					{
						if (other.fit)
						{
							this->handle = other.handle;
						}
						else
						{
							Object::CopyCreate(&this->splitResult, other.splitResult);
						}
					}
					InsertResult(InsertResult&& other) : fit(other.fit), valueRef(other.valueRef)
					{
						if (other.fit)
						{
							this->handle = other.handle;
						}
						else
						{
							Object::MoveCreate(&this->splitResult, std::move(other.splitResult));
						}
					}
					~InsertResult() { if (!this->fit) { Object::Destroy(&splitResult); } };
					InsertResult& operator=(const InsertResult& other)
					{
						if (!this->fit)
						{
							Object::Destroy(&this->splitResult);
						}

						if (other.fit)
						{
							this->handle = other.handle;
						}
						else
						{
							Object::CopyCreate(&this->splitResult, other.splitResult);
						}

						this->fit = other.fit;
						this->valueRef = other.valueRef;

						return *this;
					}
					InsertResult& operator=(InsertResult&& other)
					{
						if (!this->fit)
						{
							Object::Destroy(&this->splitResult);
						}

						if (other.fit)
						{
							this->handle = other.handle;
						}
						else
						{
							Object::MoveCreate(&this->splitResult, std::move(other.splitResult));
						}

						this->fit = other.fit;
						this->valueRef = other.valueRef;

						return *this;
					}

					bool fit;
					union
					{
						KVERef handle;           //valid if fit
						SplitResult splitResult; //valid if did not fit
					};
					V* valueRef; //non-owning
				};

				template<typename Key, typename Val>
				V* LeafInsertFit(Key&& key, Val&& value) //mark: edge
				{
					auto& nodeLen = nodeRef.node->len;
					ASSERTE(nodeLen < CAPACITY, "No space left in the leaf!");

					auto& keys = nodeRef.node->keys;
					ConstructingMoveBackwards(keys.begin()  + idx, keys.begin()    + nodeLen, keys.begin()   + nodeLen + 1u);
					::new(&keys  [idx]) K(std::forward<Key>(key));

					auto& values = nodeRef.node->values;
					ConstructingMoveBackwards(values.begin() + idx, values.begin() + nodeLen, values.begin() + nodeLen + 1u);
					::new(&values[idx]) V(std::forward<Val>(value));

					nodeLen += 1u;
					return &values[idx];
				}

				template<typename Key, typename Val>
				InsertResult LeafInsert(Key&& key, Val&& value) //mark: edge
				{
					if (nodeRef.node->len < CAPACITY)
					{
						V* const v = LeafInsertFit(std::forward<Key>(key), std::forward<Val>(value));

						return {KVERef{nodeRef, idx}, v};
					}
					else
					{
						KVERef middle = {nodeRef, B};
						auto splitResult = middle.SplitLeaf();

						NodeRef left  = splitResult.oldNodeLeft;
						NodeRef right = splitResult.newEdgeRight.AsNodeRef();

						V* const v = (idx <= B) ?
						             KVERef{left,  idx           }.LeafInsertFit(std::forward<Key>(key), std::forward<Val>(value))
						                        :
						             KVERef{right, idx - (B + 1u)}.LeafInsertFit(std::forward<Key>(key), std::forward<Val>(value));

						return {std::move(splitResult), v};
					}
				}

				template<typename Key, typename Val>
				void BranchInsertFit(Key&& key, Val&& value, const Edge edge) //mark: edge
				{
					uint16_t& nodeLen = nodeRef.node->len;
					ASSERTE(nodeLen < CAPACITY, "No space left in the branch!");
					ASSERTE(edge.height == nodeRef.height - 1u, "Attempting to insert an edge node of incorrect height. Tree corrupted!");

					auto& keys = nodeRef.node->keys;
					ConstructingMoveBackwards(keys.begin()   + idx, keys.begin()   + nodeLen, keys.begin()   + nodeLen + 1u);
					::new(&keys  [idx]) K(std::forward<Key>(key));

					auto& values = nodeRef.node->values;
					ConstructingMoveBackwards(values.begin() + idx, values.begin() + nodeLen, values.begin() + nodeLen + 1u);
					::new(&values[idx]) V(std::forward<Val>(value));

					auto& edges = nodeRef.node->AsBranch()->edges;
					std::move_backward(edges.begin() + idx + 1u, edges.begin() + nodeLen + 1u, edges.begin() + nodeLen + 2u);
					edges[idx + 1u] = edge.node;

					nodeLen += 1u;

					for (size_t i = idx + 1u; i < nodeLen + 1u; i++)
					{
						KVERef{nodeRef, i}.CorrectParentLink();
					}
				}

				template<typename Key, typename Val>
				InsertResult BranchInsert(Key&& key, Val&& value, const Edge edge) //mark: edge
				{
					ASSERTE(edge.height == nodeRef.height - 1u, "Attempting to insert an edge node of incorrect height. Tree corrupted!");

					if (nodeRef.node->len < CAPACITY)
					{
						BranchInsertFit(std::forward<Key>(key), std::forward<Val>(value), edge);

						return {KVERef{nodeRef, idx}, nullptr}; //note(vuko): returned value reference is never used and so can be safely null
					}
					else
					{
						KVERef middle = {nodeRef, B};
						auto splitResult = middle.SplitBranch();

						NodeRef left  = splitResult.oldNodeLeft;
						NodeRef right = splitResult.newEdgeRight.AsNodeRef();

						if (idx <= B)
						{
							KVERef{left,  idx           }.BranchInsertFit(std::forward<Key>(key), std::forward<Val>(value), edge);
						}
						else
						{
							KVERef{right, idx - (B + 1u)}.BranchInsertFit(std::forward<Key>(key), std::forward<Val>(value), edge);
						}

						return InsertResult(std::move(splitResult), nullptr);
					}
				}

				struct RemoveResult
				{
					K k;
					V v;
					NodeRef nodeRef; //note(vuko): technically redundant, but makes the calling code a bit easier to understand
				};
				RemoveResult LeafRemove() //mark: kv
				{
					auto& keys     = nodeRef.node->keys;
					auto& values   = nodeRef.node->values;
					auto& node_len = nodeRef.node->len;

					RemoveResult ret{std::move(keys[idx]), std::move(values[idx]), nodeRef};

					std::move(keys.begin()   + idx + 1u, keys.begin()   + node_len, keys.begin()   + idx);
					std::move(values.begin() + idx + 1u, values.begin() + node_len, values.begin() + idx);
					keys  .DestructAt(node_len - 1u);
					values.DestructAt(node_len - 1u);

					node_len -= 1u;

					return ret;
				};

				bool CanMerge() const { return KVERef{nodeRef, idx}.Descend().node->len + KVERef{nodeRef, idx + 1u}.Descend().node->len + 1u <= CAPACITY; } //mark: kv

				KVERef Merge() //mark: kv
				{
					NodeRef  leftNode   = KVERef{nodeRef, idx}.Descend();
					uint16_t& leftLen   = leftNode.node->len;
					uint16_t oldLeftLen = leftLen;

					NodeRef rightNode    = KVERef{nodeRef, idx + 1u}.Descend();
					uint16_t& rightLen   = rightNode.node->len;
					uint16_t oldRightLen = rightLen;

					ASSERTE(leftLen + 1u + rightLen <= CAPACITY, "Nodes are too big to merge!");

					auto& len = nodeRef.node->len;

					auto& keys      = nodeRef  .node->keys;
					auto& leftKeys  = leftNode .node->keys;
					auto& rightKeys = rightNode.node->keys;

					Object::MoveCreate(&leftKeys  [leftLen], std::move(keys  [idx]));
					std::move(keys.begin()   + idx + 1u, keys.begin()   + len, keys.begin()   + idx);
					keys.DestructAt(len - 1u);
					ConstructingMove(rightKeys.begin(),   rightKeys.begin()   + rightLen, leftKeys.begin()   + leftLen + 1u);

					auto& values      = nodeRef  .node->values;
					auto& leftValues  = leftNode .node->values;
					auto& rightValues = rightNode.node->values;

					Object::MoveCreate(&leftValues[leftLen], std::move(values[idx]));
					std::move(values.begin() + idx + 1u, values.begin() + len, values.begin() + idx);
					values.DestructAt(len - 1u);
					ConstructingMove(rightValues.begin(), rightValues.begin() + rightLen, leftValues.begin() + leftLen + 1u);

					auto& edges = nodeRef.node->AsBranch()->edges;
					std::move(edges.begin() + idx + 2u, edges.begin() + len + 1u, edges.begin() + idx + 1u);
					for (size_t i = idx + 1u; i < len; i++)
					{
						KVERef{nodeRef, i}.CorrectParentLink();
					}

					len -= 1u;
					leftLen += 1u + rightLen;
					rightLen = 0; //we just moved out (and destroyed) all it holds

					if (nodeRef.height > 1)
					{
						auto& rightEdges = rightNode.node->AsBranch()->edges;
						auto& leftEdges  = leftNode.node->AsBranch()->edges;
						std::move(rightEdges.begin(), rightEdges.begin() + oldRightLen + 1u, leftEdges.begin() + oldLeftLen + 1u);

						for (size_t i = oldLeftLen + 1u; i < oldLeftLen + oldRightLen + 2u; i++)
						{
							KVERef{leftNode, i}.CorrectParentLink();
						}

						delete rightNode.node->AsBranch();
					}
					else
					{
						delete rightNode.node;
					}

					return *this;
				};

				void StealLeft() //mark: kv
				{
					auto popResult = KVERef{nodeRef, idx}.Descend().PopBack();

					std::swap(nodeRef.node->keys  [idx], popResult.key);
					std::swap(nodeRef.node->values[idx], popResult.value);

					NodeRef child = KVERef{nodeRef, idx + 1u}.Descend();
					if (child.height == 0)
					{
						child.PushFront(popResult.key, popResult.value);
					}
					else
					{
						child.PushFront(popResult.key, popResult.value, popResult.edge);
					}
				}

				void StealRight() //mark: kv
				{
					auto popResult = KVERef{nodeRef, idx + 1u}.Descend().PopFront();

					std::swap(nodeRef.node->keys  [idx], popResult.key);
					std::swap(nodeRef.node->values[idx], popResult.value);

					NodeRef child = KVERef{nodeRef, idx}.Descend();
					if (child.height == 0)
					{
						child.PushBack(popResult.key, popResult.value);
					}
					else
					{
						child.PushBack(popResult.key, popResult.value, popResult.edge);
					}
				}

				void CorrectParentLink() //mark: edge
				{
					NodeRef child = this->Descend();
					child.node->parent   = this->nodeRef.node->AsBranch();
					child.node->position = static_cast<uint16_t>(this->idx);
				}
			};

			static KVERef FirstLeafEdge(NodeRef nodeRef)
			{
				while (nodeRef.height > 0)
				{
					nodeRef = KVERef{nodeRef, 0}.Descend();
				}
				return KVERef{nodeRef, 0};
			};

			static KVERef LastLeafEdge(NodeRef nodeRef)
			{
				while (nodeRef.height > 0)
				{
					nodeRef = KVERef{nodeRef, nodeRef.node->len}.Descend();
				}
				return KVERef{nodeRef, nodeRef.node->len};
			};

		};

	} //namespace Impl
} //namespace Poly
