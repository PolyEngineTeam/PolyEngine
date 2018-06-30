#pragma once

#include <unordered_map>

#include "ECS/ComponentBase.hpp"
#include "Rendering/Camera/CameraSystem.hpp"
#include "Rendering/Viewport.hpp"

namespace Poly
{
	class AARect;
	typedef size_t ViewportID;

	class ENGINE_DLLEXPORT ViewportWorldComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(ViewportWorldComponent, ComponentBase)
		{
			NO_RTTI_PROPERTY();
		}

		friend void CameraSystem::CameraUpdatePhase(World*);

	public:
		ViewportWorldComponent();

		ViewportID AddViewport(const AARect&);
		void RemoveViewport(ViewportID);
		void ResizeViewport(ViewportID, const AARect&);
		void SetCamera(ViewportID, CameraComponent*);
		CameraComponent* GetCamera(ViewportID);

		const std::unordered_map<size_t, Viewport>& GetViewports() const { return Viewports; }
	private:
		std::unordered_map<size_t, Viewport> Viewports;
		ViewportID IDCounter = 0;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ViewportWorldComponent)
}