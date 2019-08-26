#pragma once

#include <pe/Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Rendering/Camera/CameraSystem.hpp>
#include <Rendering/Viewport.hpp>

namespace pe::core::math
{
class AARect;
}

namespace Poly
{
	typedef size_t ViewportID;

	class ENGINE_DLLEXPORT ViewportWorldComponent : public ComponentBase
	{
		RTTI_DECLARE_COMPONENT(::Poly::ViewportWorldComponent) { NO_RTTI_PROPERTY(); }
	friend void CameraSystem::CameraUpdatePhase(Scene*);
	public:
		ViewportWorldComponent();

		ViewportID AddViewport(const ::pe::core::math::AARect&);
		void RemoveViewport(ViewportID);
		void ResizeViewport(ViewportID, const ::pe::core::math::AARect&);
		void SetCamera(ViewportID, CameraComponent*);

		std::unordered_map<size_t, Viewport>& GetViewports() { return Viewports; }

	private:
		std::unordered_map<size_t, Viewport> Viewports;
		ViewportID IDCounter = 0;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ViewportWorldComponent)
}