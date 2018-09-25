#include <EnginePCH.hpp>

#include <Imgui/ImguiWorldComponent.hpp>
#include <Resources/ResourceManager.hpp>
#include <Imgui/ImguiResource.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::ImguiWorldComponent)

ImguiWorldComponent::ImguiWorldComponent()
{
	gConsole.LogInfo("ImguiWorldComponent::ImguiWorldComponent");

	ImguiRes = new ImguiResource();
}

ImguiWorldComponent::~ImguiWorldComponent()
{
	delete ImguiRes;
}

ImGuiIO& ImguiWorldComponent::GetImguiIO()
{
	return ImGui::GetIO();
}

ImDrawData* ImguiWorldComponent::GetDrawData()
{
	return ImGui::GetDrawData();
}
