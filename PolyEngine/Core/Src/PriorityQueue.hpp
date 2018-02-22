#pragma once

#include "Defines.hpp"
#include "Dynarray.hpp"
#include "Optional.hpp"

#include <functional>

namespace Poly
{
	template<typename T>
	class DefaultCmp
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
			Swap(0, GetSize() - 1);
			T tmp = std::move(Data[GetSize() - 1]);
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
			while (idx > 0 && Compare(idx, GetParent(idx)))
			{
				Swap(idx, GetParent(idx));
				idx = GetParent(idx);
			}
		}

		void SiftDown(size_t idx)
		{
			while (true)
			{
				size_t leftChild = GetLeftChild(idx);
				size_t rightChild = GetRightChild(idx);

				// reached end
				if (leftChild < GetSize() && Compare(leftChild, idx))
				{
					if (rightChild < GetSize() && Compare(rightChild, leftChild))
					{
						Swap(idx, rightChild);
						idx = rightChild;
					}
					else
					{
						Swap(idx, leftChild);
						idx = leftChild;
					}
				}
				else if (rightChild < GetSize() && Compare(rightChild, idx))
				{
					Swap(idx, rightChild);
					idx = rightChild;
				}
				else
					return;
			}
		}

		inline void Swap(size_t idx1, size_t idx2)
		{
			T tmp = std::move(Data[idx1]);
			Data[idx1] = std::move(Data[idx2]);
			Data[idx2] = std::move(tmp);
		}

		inline bool Compare(size_t idx1, size_t idx2) const { return LessCmp(Data[idx1], Data[idx2]); }
		inline size_t GetParent(size_t node) { return (node - 1) / 2; }
		inline size_t GetLeftChild(size_t node) { return 2 * node + 1; }
		inline size_t GetRightChild(size_t node) { return 2 * node + 2; }

		Less LessCmp;
		Dynarray<T> Data;
	};
} //namespace Poly
