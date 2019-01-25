#include <EnginePCH.hpp>

#include <Rendering/SkyboxWorldComponent.hpp>
#include <Resources/ResourceManager.hpp>
#include <Resources/TextureResource.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::SkyboxWorldComponent)

SkyboxWorldComponent::SkyboxWorldComponent(const String& panoramaPath, const eResourceSource source)
	: Tint(Color::WHITE)
{
	Init( {panoramaPath}, source);
}

SkyboxWorldComponent::SkyboxWorldComponent(const Dynarray<String>& panoramaPaths, const eResourceSource source)
	: Tint(Color::WHITE)
{
	Init(panoramaPaths, source);
}

SkyboxWorldComponent::~SkyboxWorldComponent()
{
	for (auto p : EquirectPanoramas)
		ResourceManager<TextureResource>::Release(p);
}

void SkyboxWorldComponent::Init(const Dynarray<String>& panoramaPaths, const eResourceSource source)
{
	for (auto p : panoramaPaths)
	{
		TextureResource* texture = ResourceManager<TextureResource>::Load(p, source, eTextureUsageType::HDR);
		EquirectPanoramas.PushBack(texture);
		gConsole.LogInfo("SkyboxWorldComponent::SkyboxWorldComponent panoramaPath: {}", p);
	}
}