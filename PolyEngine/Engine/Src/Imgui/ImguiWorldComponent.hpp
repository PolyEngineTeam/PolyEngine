#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <imgui.h>

namespace Poly
{
	class ImguiResource;

	class ENGINE_DLLEXPORT ImguiWorldComponent : public ComponentBase
	{
	public:

		RTTI_DECLARE_COMPONENT(::Poly::ImguiWorldComponent) { NO_RTTI_PROPERTY(); }

		ImguiWorldComponent();
		~ImguiWorldComponent();

		ImGuiIO& GetImguiIO();
		ImDrawData* GetDrawData();

	private:
		
		ImguiResource* ImguiRes;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ImguiWorldComponent)
}