#include "CorePCH.hpp"

#include "BinaryBuffer.hpp"

using namespace Poly;

BinaryBuffer::BinaryBuffer(size_t size) : 
	Size(size)
{
	Begin = DefaultAlloc(size);
	HEAVY_ASSERTE(Begin, "Couldn't allocate memory!");
}

BinaryBuffer::~BinaryBuffer()
{
	HEAVY_ASSERTE(Begin, "");
	DefaultFree(Begin);
}

void BinaryBuffer::AddData(void* src, size_t size)
{
	void* temp = DefaultAlloc(Size + size);
	memcpy(temp, Begin, Size);
	memcpy(reinterpret_cast<void*>(reinterpret_cast<size_t>(temp) + Size), src, size);
	Size += size;
	DefaultFree(Begin);
	Begin = temp;
}
