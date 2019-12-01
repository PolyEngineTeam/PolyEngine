#pragma once

#include <pe/Defines.hpp>
#include <pe/core/rtti/RTTI.hpp>
#include <pe/api/ecs/SceneAllocator.hpp>

namespace pe::api::ecs {

	API_DLLEXPORT EntityUniquePtr EntityFactory(::Poly::RTTIBase* parent, ::Poly::RTTI::TypeInfo t);
	API_DLLEXPORT ComponentUniquePtr<ComponentBase> ComponentFactory(::Poly::RTTIBase* parent, ::Poly::RTTI::TypeInfo t);

}