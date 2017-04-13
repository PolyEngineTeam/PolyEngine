#pragma once

#include "Dynarray.hpp"

namespace Poly 
{
	template<typename T>
	class Queue : public BaseObject<>
	{
	public:
		Queue() = default;
		Queue(size_t size) : Data(size) {};

		bool Empty() { return Data.IsEmpty(); }
		size_t Size() { return Data.GetSize(); }
		T Front() { return Data.GetData()[0]; }
		T Back() { return Data.GetData()[Data.GetSize() - 1]; };
		void Push(const T &obj) { Data.PushBack(obj); }
		void Pop() { Data.PopFront(); }

	private:
		Dynarray<T> Data;
	
	};
}