#pragma once

#include <Defines.hpp>
#include <RTTI/RTTI.hpp>

namespace Poly
{
	class Entity;

	class IAction : public RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(::Poly::IAction, ::Poly::RTTIBase)
		{
			NO_RTTI_PROPERTY();
		}

	public:
		virtual void OnBegin(Entity* entity) {};
		virtual void OnUpdate(Entity* entity, float dt) = 0;
		virtual void OnAbort(Entity* entity) {};
		virtual void OnFinish(Entity* entity) {};

	}; // class IAction
} // namespace Poly