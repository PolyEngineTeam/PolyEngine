#include <EnginePCH.hpp>

#include <Rendering/SkyboxWorldComponent.hpp>
#include <Resources/ResourceManager.hpp>
#include <Resources/TextureResource.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::SkyboxWorldComponent)

SkyboxWorldComponent::SkyboxWorldComponent(const core::storage::String& panoramaPath, const eResourceSource source)
	: Tint(core::math::Color::WHITE)
{
	Init( {panoramaPath}, source);
}

SkyboxWorldComponent::SkyboxWorldComponent(const std::vector<core::storage::String>& panoramaPaths, const eResourceSource source)
	: Tint(core::math::Color::WHITE)
{
	Init(panoramaPaths, source);
}

SkyboxWorldComponent::~SkyboxWorldComponent()
{
	for (auto p : EquirectPanoramas)
		ResourceManager<TextureResource>::Release(p);
}

void SkyboxWorldComponent::Init(const std::vector<core::storage::String>& panoramaPaths, const eResourceSource source)
{
	for (auto p : panoramaPaths)
	{
		TextureResource* texture = ResourceManager<TextureResource>::Load(p, source, eTextureUsageType::HDR);
		EquirectPanoramas.push_back(texture);
		core::utils::gConsole.LogInfo("SkyboxWorldComponent::SkyboxWorldComponent panoramaPath: {}", p);
	}
}