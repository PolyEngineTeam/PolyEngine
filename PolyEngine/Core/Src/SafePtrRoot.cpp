#include "CorePCH.hpp"

#include "SafePtrRoot.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
std::unordered_map<SafePtrRoot*, size_t> SafePtrRoot::PointersMap;
Dynarray<SafePtrRoot*> SafePtrRoot::Pointers;

//------------------------------------------------------------------------------
SafePtrRoot::~SafePtrRoot()
{
	SafePtrRoot::ClearPointer(this);
}

//------------------------------------------------------------------------------
size_t SafePtrRoot::RegisterPointer(SafePtrRoot *pointer)
{
	const auto it = PointersMap.find(pointer);
	if (it != PointersMap.end())
		return it->second;
	else
	{
		SafePtrRoot::Pointers.PushBack(pointer);
		size_t idx = SafePtrRoot::Pointers.GetSize() - 1;
		SafePtrRoot::PointersMap[pointer] = idx;
		return idx;
	}
}

//------------------------------------------------------------------------------
void SafePtrRoot::ClearPointer(SafePtrRoot *pointer)
{
	size_t idx;
	try
	{
		idx = SafePtrRoot::PointersMap.at(pointer);
		SafePtrRoot::Pointers[idx] = nullptr;
		SafePtrRoot::PointersMap.erase(pointer);
	}
	catch (std::out_of_range e)
	{
		ASSERTE(false, "Pointer already cleared from safe pointer map");
	}
}

//------------------------------------------------------------------------------
SafePtrRoot *SafePtrRoot::GetPointer(size_t idx)
{
	return SafePtrRoot::Pointers[idx];
}

