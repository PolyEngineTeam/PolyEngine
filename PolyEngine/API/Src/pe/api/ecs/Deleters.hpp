#pragma once

#include <pe/Defines.hpp>

namespace pe::api::ecs {

class SceneAllocator;
class Entity;
class ComponentBase;

class API_DLLEXPORT EntityDeleter final : public ::pe::core::BaseObjectLiteralType<>
{
public:
	EntityDeleter(SceneAllocator* s) : m_sceneAllocator(s) {}
	void operator()(Entity*);
private:
	SceneAllocator* m_sceneAllocator;
};

class API_DLLEXPORT ComponentDeleter final : public ::pe::core::BaseObjectLiteralType<>
{
public:
	ComponentDeleter(SceneAllocator* s) : m_sceneAllocator(s) {}
	void operator()(ComponentBase* c);
private:
	SceneAllocator* m_sceneAllocator;
};

}