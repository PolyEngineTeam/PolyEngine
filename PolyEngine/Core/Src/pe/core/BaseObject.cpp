#include <pe/core/CorePCH.hpp>

#include <pe/core/BaseObject.hpp>
#include <pe/core/memory/Allocator.hpp>


void * ::pe::core::CustomAlocatedObject::operator new(size_t size)
{
	//TODO use custom allocator
	void* mem = ::pe::core::memory::AllocateSlab(size);
	HEAVY_ASSERTE(mem, "Couldn't allocate memory!");
	return mem;
}

void ::pe::core::CustomAlocatedObject::operator delete(void * ptr)
{
	//TODO use custom allocator
	HEAVY_ASSERTE(ptr, "");
	::pe::core::memory::Deallocate(ptr);
}
