#pragma once

#include <pe/Defines.hpp>


namespace pe::core::storage
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
		PriorityQueue(std::vector<T> data) : Data(std::move(data))
		{
			for (size_t idx = Data.size() / 2; idx > 0; --idx)
				SiftDown(idx - 1);
		}

		void Push(T val)
		{
			Data.push_back(std::move(val));
			SiftUp(Data.size() - 1);
		}

		T Pop()
		{
			T& first = Data[0];
			T& last = Data[GetSize() - 1];
			T tmp = std::move(first);
			Swap(first, last);
			Data.pop_back();
			SiftDown(0);
			return tmp;
		}

		const T& Head() const { return Data[0]; }
		size_t GetSize() const { return Data.size(); }
		void Reserve(size_t size) { Data.Reserve(size); }
	private:
		void SiftUp(size_t idx)
		{
			while (idx > 0)
			{
				T& val = Data[idx];
				const size_t parentIdx = GetParent(idx);
				T& parent = Data[parentIdx];
				if (!LessCmp(val, parent))
					break;
				
				Swap(val, parent);
				idx = parentIdx;
			}
		}

		void SiftDown(size_t idx)
		{
			while (idx < GetSize())
			{
				const size_t leftChild = GetLeftChild(idx);
				const size_t rightChild = GetRightChild(idx);

				const bool leftOk = leftChild < GetSize();
				const bool rightOk = rightChild < GetSize();

				if (!leftOk && !rightOk)
					return;

				T& val = Data[idx];
				// assign val, simple trick to bypass null reference limitations
				T* left = leftOk ? &Data[leftChild] : nullptr;
				T* right = rightOk ? &Data[rightChild] : nullptr;

				const bool rightBetter = !leftOk || (rightOk && LessCmp(*right, *left));
				T* candidate = rightBetter ? right : left;

				if (candidate && LessCmp(*candidate, val))
				{
					Swap(val, *candidate);
					idx = rightBetter ? rightChild : leftChild;
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

		inline size_t GetParent(size_t node) { return (node - 1) / 2; }
		inline size_t GetLeftChild(size_t node) { return 2 * node + 1; }
		inline size_t GetRightChild(size_t node) { return 2 * node + 2; }

		Less LessCmp;
		std::vector<T> Data;
	};
} //namespace Poly
