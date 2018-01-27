#pragma once

#include "Defines.hpp"
#include "Dynarray.hpp"

namespace Poly
{
	template <typename T>
	class PriorityQueue final : BaseObjectLiteralType<>
	{
	public:
		PriorityQueue(size_t prealocatedSize = 0) : Data(prealocatedSize) {}
		
		void Push(T val)
		{
			Data.PushBack(std::move(val));
			SiftUp(Data.GetSize() - 1);
		}

		T Pop()
		{
			Swap(0, GetSize() - 1);
			T tmp = Data.PopBack();
			SiftDown(0);
			return tmp;
		}

		size_t GetSize() const { return Data.GetSize(); }
	private:
		void SiftUp(size_t idx)
		{
			while (idx > 0 && Data[idx] < Data[GetParent(idx)])
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
				if (leftChild >= GetSize())
					return;
				else if (Data[leftChild] < Data[idx])
				{
					Swap(idx, leftChild);
					idx = leftChild;
				}
				else if (rightChild >= GetSize())
					return;
				else if (Data[rightChild] < Data[idx])
				{
					Swap(idx, rightChild);
					idx = rightChild;
				}
				else
					return;
			}
		}

		void Swap(size_t idx1, size_t idx2)
		{
			T tmp = std::move(Data[idx1]);
			Data[idx1] = std::move(Data[idx2]);
			Data[idx2] = std::move(tmp);
		}

		inline size_t GetParent(size_t node) { return (node - 1) / 2; }
		inline size_t GetLeftChild(size_t node) { return 2 * node + 1; }
		inline size_t GetRightcild(size_t node) { return 2 * node + 2; }

		Dynarray<T> Data;
	};
} //namespace Poly
