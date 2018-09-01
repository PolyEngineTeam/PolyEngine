#pragma once

#include <bitset>
#include "Dynarray.hpp"

namespace Poly {
	class CORE_DLLEXPORT BitMask
	{
	public:
		using DataType = unsigned short;

		BitMask(size_t size = sizeof(DataType));

		//Bitwise operators
		BitMask operator|(const BitMask& rhs) const;
		BitMask operator^(const BitMask& rhs) const;
		BitMask operator&(const BitMask& rhs) const;
		BitMask operator~() const;
		//Bitwise assignment operators
		BitMask& operator|=(const BitMask& rhs);
		BitMask& operator^=(const BitMask& rhs);
		BitMask& operator&=(const BitMask& rhs);


		bool operator==(const BitMask rhs) const;
		bool operator!=(const BitMask& rhs) const
		{
			return !(*this == rhs);
		}


		//@todo Convert this method to return bit proxy 
		bool operator[](size_t index) const;

		//Set,Reset and toggle bits methods
		bool Set(size_t index, bool state);
		bool Reset();
		bool Toggle(size_t index);

		bool Resize(size_t size = 0);


		size_t GetSize() { return BitsNumber; }
		size_t GetDynarraySize() { return BitList.GetSize(); }
	protected:
		
	private:
		inline size_t BitListIndex(size_t index);

		Dynarray<DataType> BitList;
		size_t BitsNumber = 0;				//How many bits are in the class
	};


	class CORE_DLLEXPORT BitMaskProxy : public BitMask {
	private:
		bool bitValue;
		size_t bitIndex;
	public:
		BitMaskProxy(size_t size = sizeof(DataType)) : BitMask(size) {}
		operator bool()		{ return bitValue; }
		BitMaskProxy operator[](size_t index);
		BitMaskProxy operator=(bool);

	};
}