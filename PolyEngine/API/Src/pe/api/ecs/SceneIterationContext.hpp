#pragma once

#include <pe/Defines.hpp>
#include <pe/api/ecs/ComponentIterator.hpp>
#include <pe/api/ecs/SceneAllocator.hpp>

namespace pe::api::ecs {

template<typename PrimaryComponent, typename... SecondaryComponents>
class SceneIterationContext final : core::BaseObjectLiteralType<>
{
public:
	SceneIterationContext(SceneAllocator* alloc) : m_sceneAlloc(alloc) {}
	[[nodiscard]] ComponentIterator<PrimaryComponent, SecondaryComponents...> begin()
	{
		auto begin = m_sceneAlloc->componentsBegin<PrimaryComponent>();
		auto end = m_sceneAlloc->componentsEnd<PrimaryComponent>();
		return ComponentIterator<PrimaryComponent, SecondaryComponents...>(begin, begin, end);
	}
	[[nodiscard]] ComponentIterator<PrimaryComponent, SecondaryComponents...> end()
	{
		auto begin = m_sceneAlloc->componentsBegin<PrimaryComponent>();
		auto end = m_sceneAlloc->componentsEnd<PrimaryComponent>();
		return ComponentIterator<PrimaryComponent, SecondaryComponents...>(end, begin, end);
	}

private:
	SceneAllocator* const m_sceneAlloc;
};

}