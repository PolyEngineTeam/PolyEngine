#include <EnginePCH.hpp>

#include <Resources/CubemapResource.hpp>
#include <Resources/ResourceManager.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

CubemapResource::CubemapResource(const core::utils::EnumArray<core::storage::String, eCubemapSide> paths)
{
	ASSERTE(paths.GetSize() == 6, "CubemapResource::CubemapResource parths need to have 6 elements");
	core::utils::gConsole.LogInfo("CubemapResource::CubemapResource path:{}", paths[eCubemapSide::LEFT]);

	for (auto side : core::utils::IterateEnum<eCubemapSide>())
	{
		core::storage::String absolutePath = gAssetsPathConfig.GetAssetsPath(eResourceSource::ENGINE) + paths[side];
		
		int fileChannels;
		int fileWidth;
		int fileHeight;

		Images[side] = LoadImageHDR(absolutePath, &fileWidth, &fileHeight, &fileChannels);

		Width = std::max(Width, fileWidth);
		Height = std::max(Height, fileHeight);
	}

	TextureProxy = gEngine->GetRenderingDevice()->CreateCubemap(Width, Height);

	for (auto side : core::utils::IterateEnum<eCubemapSide>())
	{
		TextureProxy->SetContentHDR(side, Images[side]);
	}

	for (auto side : core::utils::IterateEnum<eCubemapSide>())
	{
		FreeImageHDR(Images[side]);
	}
}

CubemapResource::~CubemapResource()
{
}
