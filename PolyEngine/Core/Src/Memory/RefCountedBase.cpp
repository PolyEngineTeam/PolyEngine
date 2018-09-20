#include <CorePCH.hpp>

#include <Memory/RefCountedBase.hpp>

using namespace Poly;

//------------------------------------------------------------------------------
void RefCountedBase::AddRef()
{
	++RefCount;
	HEAVY_ASSERTE(RefCount > 0, "Reference counter overflow!");
}

//------------------------------------------------------------------------------
bool RefCountedBase::RemoveRef()
{
	HEAVY_ASSERTE(RefCount > 0, "Cannot remove any more references!");
	--RefCount;
	return RefCount == 0;
}

//------------------------------------------------------------------------------
RefCountedBase::~RefCountedBase()
{
	HEAVY_ASSERTE(RefCount == 0, "Ref counted object was removed but reference count was greater than 0!");
}
