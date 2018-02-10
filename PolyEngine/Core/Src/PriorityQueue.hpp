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
		PriorityQueue(std::function<bool(const T&, const T&)> cmp, size_t prealocatedSize = 0) : Cmp(std::move(cmp)), Data(prealocatedSize) {}
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
		void SetLessCmp(std::function<bool(const T&, const T&)> less) { Cmp = std::move(less); }
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

		inline bool Compare(size_t idx1, size_t idx2) const { return Cmp(Data[idx1], Data[idx2]); }
		inline size_t GetParent(size_t node) { return (node - 1) / 2; }
		inline size_t GetLeftChild(size_t node) { return 2 * node + 1; }
		inline size_t GetRightChild(size_t node) { return 2 * node + 2; }

		Dynarray<T> Data;
		std::function<bool(const T&, const T&)> Cmp = [](const T& a, const T& b) { return a < b; };
	};
} //namespace Poly
