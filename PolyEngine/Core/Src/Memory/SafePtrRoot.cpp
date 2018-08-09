#include "CorePCH.hpp"

#include "SafePtrRoot.hpp"

RTTI_DEFINE_TYPE(::Poly::SafePtrRoot);

using namespace Poly;

//------------------------------------------------------------------------------
std::unordered_map<SafePtrRoot*, size_t> SafePtrRoot::PointersMap;
Dynarray<SafePtrRoot*> SafePtrRoot::Pointers;

//------------------------------------------------------------------------------
SafePtrRoot::~SafePtrRoot()
{
	SafePtrRoot::ClearPointer(this);
}

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

/// <summary>Registers pointer in array and map</summary>
/// <param name="pointer">Pointer to be registered</param>
/// <returns>Index of given pointer in array</returns>
void SafePtrRoot::ClearPointer(SafePtrRoot *pointer)
{
	HEAVY_ASSERTE(pointer != nullptr, "Cannot unregister nullptr");

	const auto it = SafePtrRoot::PointersMap.find(pointer);
	if (it != SafePtrRoot::PointersMap.end())
	{
		SafePtrRoot::Pointers[it->second] = nullptr;
		SafePtrRoot::PointersMap.erase(it);
	}
}

//------------------------------------------------------------------------------
SafePtrRoot *SafePtrRoot::GetPointer(size_t idx)
{
	return SafePtrRoot::Pointers[idx];
}

