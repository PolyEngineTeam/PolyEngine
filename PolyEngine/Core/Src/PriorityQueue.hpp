#pragma once

#include "Defines.hpp"
#include "Dynarray.hpp"
#include "Optional.hpp"

#include <functional>

namespace Poly
{
	template<typename T>
	struct DefaultCmp
	{
		bool operator()(const T& a, const T& b) const { return a < b; }
	};


	template <typename T, typename Less = DefaultCmp<T>>
	class PriorityQueue final : BaseObjectLiteralType<>
	{
	public:
		PriorityQueue(size_t prealocatedSize = 0) : Data(prealocatedSize) {}
		PriorityQueue(Less lessCmp, size_t prealocatedSize = 0) : LessCmp(std::move(lessCmp)), Data(prealocatedSize) {}
		PriorityQueue(Dynarray<T> data) : Data(std::move(data))
		{
			for (size_t idx = Data.GetSize() / 2; idx > 0; --idx)
				SiftDown(idx - 1);
		}

		void Push(T val)
		{
			Data.PushBack(std::move(val));
			SiftUp(Data.GetSize() - 1);
		}

		T Pop()
		{
			T& first = Data[0];
			T& last = Data[GetSize() - 1];
			T tmp = std::move(first);
			Swap(first, last);
			Data.PopBack();
			SiftDown(0);
			return tmp;
		}

		const T& Head() const { return Data[0]; }
		size_t GetSize() const { return Data.GetSize(); }
		void Reserve(size_t size) { Data.Reserve(size); }
	private:
		void SiftUp(size_t idx)
		{
			while (idx > 0)
			{
				T& val = Data[idx];
				const size_t parentIdx = GetParent(idx);
				T& parent = Data[parentIdx];
				if (!Compare(val, parent))
					break;
				
				Swap(val, parent);
				idx = parentIdx;
			}
		}

		void SiftDown(size_t idx)
		{
			while (idx < GetSize())
			{
				size_t leftChild = GetLeftChild(idx);
				size_t rightChild = GetRightChild(idx);

				T& val = Data[idx];
				T* left = leftChild < GetSize() ? &Data[leftChild] : nullptr;
				T* right = rightChild < GetSize() ? &Data[rightChild] : nullptr;

				// reached end
				if (left && Compare(*left, val))
				{
					if (right && Compare(*right, *left))
					{
						Swap(val, *right);
						idx = rightChild;
					}
					else
					{
						Swap(val, *left);
						idx = leftChild;
					}
				}
				else if (right && Compare(*right, val))
				{
					Swap(val, *right);
					idx = rightChild;
				}
				else
					return;
			}
		}

		inline void Swap(T& a, T& b)
		{
			T tmp = std::move(a);
			a = std::move(b);
			b = std::move(tmp);
		}

		inline bool Compare(const T& a, const T& b) const { return LessCmp(a, b); }
		inline size_t GetParent(size_t node) { return (node - 1) / 2; }
		inline size_t GetLeftChild(size_t node) { return 2 * node + 1; }
		inline size_t GetRightChild(size_t node) { return 2 * node + 2; }

		Less LessCmp;
		Dynarray<T> Data;
	};
} //namespace Poly
