#include <pe/api/APIPCH.hpp>
#include <pe/api/ecs/ComponentIDGenerator.hpp>

namespace pe::api::ecs {

ComponentManager& ComponentManager::Get()
{
	static ComponentManager instance;
	return instance;
}

std::optional<size_t> ComponentManager::GetComponentID(const ::Poly::RTTI::TypeInfo& typeinfo) const
{
	const auto& it = TypeToIDMap.find(typeinfo);
	if (it == TypeToIDMap.end())
		return {};
	return it->second;
}

::Poly::RTTI::TypeInfo ComponentManager::GetComponentType(size_t id) const
{
	const auto& it = IDToTypeMap.find(id);
	if (it == IDToTypeMap.end())
		return ::Poly::RTTI::TypeInfo::INVALID;
	return it->second;
}

std::vector<std::pair<::Poly::RTTI::TypeInfo, size_t>> ComponentManager::GetComponentTypesList() const
{
	std::vector<std::pair<::Poly::RTTI::TypeInfo, size_t>> ret;
	ret.reserve(TypeToIDMap.size());

	for (const auto& kv : TypeToIDMap)
		ret.push_back({kv.first, kv.second});

	return ret;
}

std::unique_ptr<core::memory::IterablePoolAllocatorBase> ComponentManager::CreateAllocator(size_t id, size_t componentCount) const
{
	const auto& it = IDToCreatorMap.find(id);
	if (it == IDToCreatorMap.end())
		return nullptr;
	return std::unique_ptr<core::memory::IterablePoolAllocatorBase>(it->second(componentCount));
}

void ComponentManager::Clear()
{
	TypeToIDMap.clear();
	IDToTypeMap.clear();
	IDToCreatorMap.clear();
}

size_t impl::ComponentIDGenerator::GenerateID() noexcept
{
	static size_t value = 0;
	return value++;
}

}