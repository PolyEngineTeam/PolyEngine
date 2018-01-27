#pragma once

#include "Defines.hpp"
#include "Dynarray.hpp"
#include "Optional.hpp"

#include <functional>

namespace Poly
{
	template <typename T>
	class PriorityQueue final : BaseObjectLiteralType<>
	{
	public:
		PriorityQueue(size_t prealocatedSize = 0) : Data(prealocatedSize) {}
		PriorityQueue(std::function<bool(const T&, const T&)> less, size_t prealocatedSize = 0) : Data(prealocatedSize), Less(std::move(less)) {}
		
		void Push(T val)
		{
			Data.PushBack(std::move(val));
			SiftUp(Data.GetSize() - 1);
		}

		T Pop()
		{
			Swap(0, GetSize() - 1);
			T tmp = Data[GetSize() - 1];
			Data.PopBack();
			SiftDown(0);

			return tmp;
		}

		size_t GetSize() const { return Data.GetSize(); }
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

		void Swap(size_t idx1, size_t idx2)
		{
			T tmp = std::move(Data[idx1]);
			Data[idx1] = std::move(Data[idx2]);
			Data[idx2] = std::move(tmp);
		}

		bool Compare(size_t idx1, size_t idx2)
		{
			if (Less.HasValue())
				return Less.Value()(Data[idx1], Data[idx2]);
			else
				return Data[idx1] < Data[idx2];
		}

		inline size_t GetParent(size_t node) { return (node - 1) / 2; }
		inline size_t GetLeftChild(size_t node) { return 2 * node + 1; }
		inline size_t GetRightChild(size_t node) { return 2 * node + 2; }

		Dynarray<T> Data;
		Optional<std::function<bool(const T&, const T&)>> Less;
	};
} //namespace Poly
