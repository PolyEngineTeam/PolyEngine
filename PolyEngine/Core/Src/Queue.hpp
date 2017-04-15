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

		bool Empty() const { return Data.IsEmpty(); }
		size_t Size() const { return Data.GetSize(); }
		const T& Front() const { return Data.GetData()[0]; }
		const T& Back() const { return Data.GetData()[Data.GetSize() - 1]; };
		T& Front() { return Data.GetData()[0]; }
		T& Back() { return Data.GetData()[Data.GetSize() - 1]; };
		void Push(const T &obj) { Data.PushBack(obj); }
		void Pop() { Data.PopFront(); }
	private:
		Dynarray<T> Data;
	};
}