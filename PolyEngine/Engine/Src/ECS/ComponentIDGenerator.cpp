#include <EnginePCH.hpp>
#include <ECS/ComponentIDGenerator.hpp>

using namespace Poly;

Poly::ComponentManager& Poly::ComponentManager::Get()
{
	static ComponentManager instance;
	return instance;
}

Optional<size_t> Poly::ComponentManager::GetComponentID(const RTTI::TypeInfo& typeinfo) const
{
	const auto& it = TypeToIDMap.find(typeinfo);
	if (it == TypeToIDMap.end())
		return {};
	return it->second;
}

Poly::RTTI::TypeInfo Poly::ComponentManager::GetComponentType(size_t id) const
{
	const auto& it = IDToTypeMap.find(id);
	if (it == IDToTypeMap.end())
		return RTTI::TypeInfo::INVALID;
	return it->second;
}

Dynarray<std::pair<RTTI::TypeInfo, size_t>> Poly::ComponentManager::GetComponentTypesList() const
{
	Dynarray<std::pair<RTTI::TypeInfo, size_t>> ret;
	ret.Reserve(TypeToIDMap.size());

	for (const auto& kv : TypeToIDMap)
		ret.PushBack({kv.first, kv.second});

	return ret;
}

IterablePoolAllocatorBase* Poly::ComponentManager::CreateAllocator(size_t id, size_t componentCount) const
{
	const auto& it = IDToCreatorMap.find(id);
	if (it == IDToCreatorMap.end())
		return nullptr;
	return it->second(componentCount);
}

void Poly::ComponentManager::Clear()
{
	TypeToIDMap.clear();
	IDToTypeMap.clear();
	IDToCreatorMap.clear();
}

size_t Poly::Impl::ComponentIDGenerator::GenerateID() noexcept
{
	static size_t value = 0;
	return value++;
}