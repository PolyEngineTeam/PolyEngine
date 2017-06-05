#include "EnginePCH.hpp"

using namespace Poly;

ViewportWorldComponent::ViewportWorldComponent()
{
	AddViewport(AABox(Vector(0, 0, 0), Vector(1, 1, 0)));
}

ViewportID ViewportWorldComponent::AddViewport(const AABox& rect)
{
	ViewportID id = IDCounter++;
	Viewport view(rect);
	Viewports.insert(std::make_pair(id, view));

	return id;
}

void ViewportWorldComponent::RemoveViewport(ViewportID id)
{
	auto it = Viewports.find(id);
	ASSERTE(it != Viewports.end(), "Viewport doesn't exist.");
	Viewports.erase(it);
}

void ViewportWorldComponent::ResizeViewport(ViewportID id, const AABox& rect)
{
	auto it = Viewports.find(id);
	ASSERTE(it != Viewports.end(), "Viewport doesn't exist.");
	it->second.Resize(rect);
}

void ViewportWorldComponent::SetCamera(ViewportID id, CameraComponent* cam)
{
	auto it = Viewports.find(id);
	ASSERTE(it != Viewports.end(), "Viewport doesn't exist.");
	it->second.SetCamera(cam);
}
