#include "CorePCH.hpp"

#include "Allocator.hpp"
#include "BaseObject.hpp"

void * Poly::CustomAlocatedObject::operator new(size_t size)
{
	//TODO use custom allocator
	void* mem = DefaultAlloc(size);
	HEAVY_ASSERTE(mem, "Couldn't allocate memory!");
	return mem;
}

void Poly::CustomAlocatedObject::operator delete(void * ptr)
{
	//TODO use custom allocator
	HEAVY_ASSERTE(ptr, "");
	DefaultFree(ptr);
}