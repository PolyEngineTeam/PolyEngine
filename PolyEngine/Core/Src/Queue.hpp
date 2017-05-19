#pragma once

#include "Dynarray.hpp"

namespace Poly 
{
	template<typename T>
	class Queue : public BaseObject<>
	{
	public:
		Queue() = default;
		Queue(size_t size) : Data(size), RingCap(size), BuffCap(size)
		{
			for (size_t i = 0; i < size; i++)
			{
				Data.PushBack(T());
			}
		}

		bool Empty() const { return BuffSize == 0; }
		size_t Size() const { return BuffSize; }
		const T& Front() const { return Data.GetData()[Head]; }
		const T& Back() const 
		{
			if (RingCap == BuffCap)
			{
				if (Tail - 1 < 0)
					return Data.GetData()[RingCap - 1];
				else
					return Data.GetData()[Tail -1];
			}
			else
				return Data.GetData()[BuffCap - 1];
		}
		T& Front() { return Data.GetData()[Head]; }
		T& Back()
		{
			if (RingCap == BuffCap)
			{
				if (Tail - 1 < 0)
					return Data.GetData()[RingCap - 1];
				else
					return Data.GetData()[Tail -1];
			}
			else
				return Data.GetData()[BuffCap - 1];
		}
		void Push(const T &obj) 
		{
			if (RingSize == RingCap || RingCap != BuffCap)
			{
				Data.PushBack(obj);
				BuffSize++;
				BuffCap++;
				if (Tail == 0 && RingSize > 0) //Check wheather ring can be expanded
				{
					RingSize = BuffSize;
					RingCap = BuffCap;
				}
			}
			else
			{
				Data.GetData()[Tail] = obj;
				Tail = (Tail + 1) % RingCap;
				RingSize++;
				BuffSize++;
			}
		}
		void Pop() 
		{
			if (!Empty())
			{
				RingSize--;
				BuffSize--;
				if ((RingSize == 0 && BuffSize != 0) || RingCap == 0) //Check wheather ring can be expanded
				{
					Head = RingCap;
					Tail = 0;
					RingSize = BuffSize;
					RingCap = BuffCap;
				}
				else if (RingSize != 0)
					Head = (Head + 1) % RingCap;
			}
			 
		}
	private:
		Dynarray<T> Data;
		int Head = 0;
		int Tail = 0;
		size_t RingCap = 0;
		size_t BuffCap = 0;
		size_t RingSize = 0;
		size_t BuffSize = 0;
	};
}