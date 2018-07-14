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
	friend void CameraSystem::CameraUpdatePhase(Scene*);
	public:
		ViewportWorldComponent();

		ViewportID AddViewport(const AARect&);
		void RemoveViewport(ViewportID);
		void ResizeViewport(ViewportID, const AARect&);
		void SetCamera(ViewportID, CameraComponent*);

		const std::unordered_map<size_t, Viewport>& GetViewports() const { return Viewports; }
	private:
		std::unordered_map<size_t, Viewport> Viewports;
		ViewportID IDCounter = 0;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ViewportWorldComponent)
}