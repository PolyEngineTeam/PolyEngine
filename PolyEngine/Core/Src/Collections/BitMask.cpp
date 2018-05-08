#include "CorePCH.hpp"

#include <climits>

constexpr u64 ZERO = 0UL;

using namespace Poly;

constexpr BitMask::DataType TYPE_BIT = CHAR_BIT * sizeof(BitMask::DataType);

BitMask::BitMask(size_t size)
	: BitsNumber(size)
{
	size_t arraySize = 0;

	if (size%TYPE_BIT)
		arraySize = size / TYPE_BIT + 1;
	else
		arraySize = size / TYPE_BIT;

	for (int i = 0; i < arraySize; i++)
		BitList.PushBack(ZERO);
}

bool BitMask::Reset()
{
	BitList.Clear();
	BitsNumber = 0;
	return true;
}

bool BitMask::Toggle(size_t index)
{
	HEAVY_ASSERTE(RangeCheck(index), "Out of bounds");
	BitList[index / TYPE_BIT] ^= 1UL << index%TYPE_BIT;
	return true;
}

bool BitMask::operator[](size_t index) const
{
	DataType tempChar = BitList[index / TYPE_BIT];
	tempChar = (tempChar >> index%TYPE_BIT) & 1UL;
	return tempChar != 0;
}

bool BitMask::Set(size_t index, bool state)
{
	HEAVY_ASSERTE(RangeCheck(index), "Out of bounds");

	size_t bitListIndex = index / TYPE_BIT;
	size_t bitPosition = index % TYPE_BIT;

	if (state)
		BitList[bitListIndex] |= (1UL << bitPosition);
	else
		BitList[bitListIndex] &= ~(1UL << bitPosition);
	return true;
}

bool BitMask::RangeCheck(size_t index)
{
	if (index >= BitsNumber || index < 0)
		return false;
	return true;
}

BitMask BitMask::operator|(const BitMask& rhs) const
{
	//Equal Dynarray sizes
	if (BitList.GetSize() == rhs.BitList.GetSize())
	{
		size_t tempBitsNumber = 0;
		if (BitsNumber > rhs.BitsNumber)
			tempBitsNumber = BitsNumber;
		else
			tempBitsNumber = rhs.BitsNumber;

		BitMask temp(tempBitsNumber);
		for (size_t i = 0; i < BitList.GetSize(); i++)
			temp.BitList[i] = BitList[i] | rhs.BitList[i];
		return temp;
	}

	else if (BitList.GetSize() > rhs.BitList.GetSize())
	{
		BitMask temp(BitsNumber);
		for (size_t i = 0; i < rhs.BitList.GetSize(); i++)
			temp.BitList[i] = BitList[i] | rhs.BitList[i];
		for (size_t i = rhs.BitList.GetSize(); i < BitList.GetSize(); i++)
			temp.BitList[i] = BitList[i];
		return temp;
	}

	else if (BitList.GetSize() < rhs.BitList.GetSize())
	{
		BitMask temp(rhs.BitsNumber);
		for (size_t i = 0; i < BitList.GetSize(); i++)
			temp.BitList[i] = BitList[i] | rhs.BitList[i];
		for (size_t i = BitList.GetSize(); i < rhs.BitList.GetSize(); i++)
			temp.BitList[i] = rhs.BitList[i];
		return temp;
	}
	return BitMask(0);
}

BitMask BitMask::operator^(const BitMask& rhs) const
{
	if (BitList.GetSize() == rhs.BitList.GetSize())
	{
		size_t tempBitsNumber = 0;
		if (BitsNumber > rhs.BitsNumber)
			tempBitsNumber = BitsNumber;
		else
			tempBitsNumber = rhs.BitsNumber;

		BitMask temp(tempBitsNumber);
		for (size_t i = 0; i < BitList.GetSize(); i++)
			temp.BitList[i] = BitList[i] ^ rhs.BitList[i];
		return temp;
	}
	//Lhs size bigger than rhs size
	else if (BitList.GetSize() > rhs.BitList.GetSize())
	{
		BitMask temp(BitsNumber);
		for (size_t i = 0; i < rhs.BitList.GetSize(); i++)
			temp.BitList[i] = BitList[i] ^ rhs.BitList[i];
		for (size_t i = rhs.BitList.GetSize(); i < BitList.GetSize(); i++)
			temp.BitList[i] = BitList[i];
		return temp;
	}
	//rhs size bigger than lhs size
	else if (BitList.GetSize() < rhs.BitList.GetSize())
	{
		BitMask temp(rhs.BitsNumber);
		for (size_t i = 0; i < BitList.GetSize(); i++)
			temp.BitList[i] = BitList[i] ^ rhs.BitList[i];
		for (size_t i = BitList.GetSize(); i < rhs.BitList.GetSize(); i++)
			temp.BitList[i] = rhs.BitList[i];
		return temp;
	}
	return BitMask(0);
}

BitMask BitMask::operator&(const BitMask& rhs) const
{
	if (BitList.GetSize() == rhs.BitList.GetSize())
	{
		size_t tempBitsNumber = 0;
		if (BitsNumber > rhs.BitsNumber)
			tempBitsNumber = BitsNumber;
		else
			tempBitsNumber = rhs.BitsNumber;

		BitMask temp(tempBitsNumber);
		for (size_t i = 0; i < BitList.GetSize(); i++)
			temp.BitList[i] = BitList[i] & rhs.BitList[i];
		return temp;
	}
	else if (BitList.GetSize() > rhs.BitList.GetSize())
	{
		BitMask temp(BitsNumber);
		for (size_t i = 0; i < rhs.BitList.GetSize(); i++)
			temp.BitList[i] = BitList[i] & rhs.BitList[i];
		for (size_t i = rhs.BitList.GetSize(); i < BitList.GetSize(); i++)
			temp.BitList[i] = ZERO;
		return temp;
	}
	else if (BitList.GetSize() < rhs.BitList.GetSize())
	{
		BitMask temp(rhs.BitsNumber);
		for (size_t i = 0; i < BitList.GetSize(); i++)
			temp.BitList[i] = BitList[i] & rhs.BitList[i];
		for (size_t i = BitList.GetSize(); i < rhs.BitList.GetSize(); i++)
			temp.BitList[i] = ZERO;
		return temp;
	}
	return BitMask(0);
}

BitMask& BitMask::operator~()
{
	for (auto& x : BitList)
		x = ~x;

	return *this;
}


bool BitMask::Resize(const int offset)
{
	if (offset > 0)
	{
		if (BitsNumber + offset <= GetDynarraySize()*TYPE_BIT)
		{
			BitsNumber += offset;
			return true;
		}
		else
		{
			size_t currentSize = BitList.GetSize();
			size_t targetSize = 0;
			if ((BitsNumber + offset) % TYPE_BIT)
				targetSize = (BitsNumber + offset) / TYPE_BIT + 1;
			else
				targetSize = (BitsNumber + offset) / TYPE_BIT;

			size_t pushBackCount = targetSize - currentSize;

			for (size_t i = 0; i < pushBackCount; i++)
				BitList.PushBack(ZERO);

			BitsNumber += offset;
			return true;
		}
	}

	if (offset < 0)
	{
		HEAVY_ASSERTE(BitsNumber + offset >= 0 && BitsNumber + offset<BitsNumber, "Out of bounds");
		if (BitsNumber + offset >(GetDynarraySize() - 1)*TYPE_BIT)
		{
			BitsNumber += offset;
			return true;
		}
		else
		{
			size_t currentSize = BitList.GetSize();
			size_t targetSize = 0;
			if (-1 * (BitsNumber + offset) % TYPE_BIT)
				targetSize = (BitsNumber + offset) / TYPE_BIT + 1;
			else
				targetSize = (BitsNumber + offset) / TYPE_BIT;

			size_t popBackCount = currentSize - targetSize;
			for (size_t i = 0; i < popBackCount; i++)
				BitList.PopBack();

			BitsNumber += offset;
			return true;
		}
	}


	if (offset == 0)
	{
		return false;
	}
	return false;
}

size_t BitMask::BitListIndex(size_t index)
{
	return index / TYPE_BIT;
}

BitMask& BitMask::operator|=(const BitMask& rhs)
{
	//Equal Dynarray sizes
	if (BitList.GetSize() == rhs.BitList.GetSize())
	{
		for (size_t i = 0; i < BitList.GetSize(); i++)
			BitList[i] |= rhs.BitList[i];

		if (rhs.BitsNumber > BitsNumber)
			BitsNumber = rhs.BitsNumber;

		return *this;
	}

	else if (BitList.GetSize() > rhs.BitList.GetSize())
	{
		for (size_t i = 0; i < rhs.BitList.GetSize(); i++)
			BitList[i] |= rhs.BitList[i];

		return *this;
	}

	else if (BitList.GetSize() < rhs.BitList.GetSize())
	{
		for (size_t i = 0; i < BitList.GetSize(); i++)
			BitList[i] |= rhs.BitList[i];

		size_t oldBitListSize = BitList.GetSize();

		for (size_t i = 0; i < rhs.BitList.GetSize() - oldBitListSize; i++)
			BitList.PushBack(ZERO);

		for (size_t i = oldBitListSize; i < BitList.GetSize(); i++)
			BitList[i] = rhs.BitList[i];

		BitsNumber = rhs.BitsNumber;
		return *this;
	}
	return *this;
}

BitMask& BitMask::operator^=(const BitMask& rhs)
{
	if (BitList.GetSize() == rhs.BitList.GetSize())
	{
		for (size_t i = 0; i < BitList.GetSize(); i++)
			BitList[i] ^= rhs.BitList[i];

		if (rhs.BitsNumber > BitsNumber)
			BitsNumber = rhs.BitsNumber;

		return *this;
	}
	else if (BitList.GetSize() > rhs.BitList.GetSize())
	{
		for (size_t i = 0; i < rhs.BitList.GetSize(); i++)
			BitList[i] ^= rhs.BitList[i];

		return *this;
	}
	else if (BitList.GetSize() < rhs.BitList.GetSize())
	{
		for (size_t i = 0; i < BitList.GetSize(); i++)
			BitList[i] ^= rhs.BitList[i];

		size_t oldBitListSize = BitList.GetSize();

		for (size_t i = 0; i < rhs.BitList.GetSize() - oldBitListSize; i++)
			BitList.PushBack(ZERO);

		for (size_t i = oldBitListSize; i < BitList.GetSize(); i++)
			BitList[i] = rhs.BitList[i];

		BitsNumber = rhs.BitsNumber;
		return *this;
	}
	return *this;
}

BitMask& BitMask::operator&=(const BitMask& rhs)
{

	if (BitList.GetSize() == rhs.BitList.GetSize())
	{
		for (size_t i = 0; i < BitList.GetSize(); i++)
			BitList[i] &= rhs.BitList[i];

		if (rhs.BitsNumber > BitsNumber)
			BitsNumber = rhs.BitsNumber;

		return *this;
	}
	else if (BitList.GetSize() > rhs.BitList.GetSize())
	{
		for (size_t i = 0; i < rhs.BitList.GetSize(); i++)
			BitList[i] &= rhs.BitList[i];

		for (size_t i = rhs.BitList.GetSize(); i < BitList.GetSize(); i++)
			BitList[i] = ZERO;
		return *this;
	}
	else if (BitList.GetSize() < rhs.BitList.GetSize())
	{
		for (size_t i = 0; i < BitList.GetSize(); i++)
			BitList[i] &= rhs.BitList[i];

		size_t oldBitListSize = BitList.GetSize();

		for (size_t i = 0; i < rhs.BitList.GetSize() - oldBitListSize; i++)
			BitList.PushBack(ZERO);

		for (size_t i = oldBitListSize; i < BitList.GetSize(); i++)
			BitList[i] = ZERO;

		BitsNumber = rhs.BitsNumber;
		return *this;
	}
	return *this;
}

bool BitMask::operator==(const BitMask rhs) const
{
	if (BitsNumber != rhs.BitsNumber)
		return false;

	if (BitsNumber == rhs.BitsNumber)
	{
		for (size_t i = 0; i < BitList.GetSize(); i++)
			if (BitList[i] != rhs.BitList[i])
				return false;
		return true;
	}
	return false;
}