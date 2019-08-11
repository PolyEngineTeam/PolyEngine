#include <pe/core/CorePCH.hpp>

#include <pe/core/memory/BinaryBuffer.hpp>
#include <pe/core/memory/Allocator.hpp>

using namespace pe::core::memory;

BinaryBuffer::BinaryBuffer(size_t size)
	: Size(size)
{
	HEAVY_ASSERTE(size > 0, "Creating buffer with size == 0!");
	Data = AllocateSlab(size);
	HEAVY_ASSERTE(Data, "Couldn't allocate memory!");
}

BinaryBuffer::~BinaryBuffer()
{
	if(Data)
		Deallocate(Data);
}

BinaryBuffer& Poly::BinaryBuffer::operator=(const BinaryBuffer& rhs)
{
	if(Data)
		Deallocate(Data);
	Data = AllocateSlab(rhs.Size);
	memcpy(Data, rhs.Data, rhs.Size);
	Size = rhs.Size;
	return *this;
}

BinaryBuffer& Poly::BinaryBuffer::operator=(BinaryBuffer&& rhs)
{
	if (Data)
		Deallocate(Data);
	Data = rhs.Data;
	Size = rhs.Size;
	rhs.Data = nullptr;
	rhs.Size = 0;
	return *this;
}
