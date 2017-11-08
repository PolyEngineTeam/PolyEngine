#include "CorePCH.hpp"

#include "SafePtrRoot.hpp"

using namespace Poly;

//TODO remove later
int SafePtrRoot::Debug()
{
	return 5;
}

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
	SafePtrRoot::Pointers.PushBack(pointer);
	size_t idx = SafePtrRoot::Pointers.GetSize() - 1;
	SafePtrRoot::PointersMap[pointer] = idx;
	//return &SafePtrRoot::Pointers[idx];
	return idx;
}

//------------------------------------------------------------------------------
void SafePtrRoot::ClearPointer(SafePtrRoot *pointer)
{
	size_t idx = SafePtrRoot::PointersMap[pointer];
	SafePtrRoot::Pointers[idx] = nullptr;
	SafePtrRoot::PointersMap.erase(pointer);
}

//------------------------------------------------------------------------------
SafePtrRoot *SafePtrRoot::GetPointer(size_t idx)
{
	return SafePtrRoot::Pointers[idx];
}

