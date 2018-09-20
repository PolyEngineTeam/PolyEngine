#include <EnginePCH.hpp>

#include <Resources/CubemapResource.hpp>
#include <Resources/ResourceManager.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

CubemapResource::CubemapResource(const EnumArray<String, eCubemapSide> paths)
{
	ASSERTE(paths.GetSize() == 6, "CubemapResource::CubemapResource parths need to have 6 elements");
	gConsole.LogInfo("CubemapResource::CubemapResource path:{}", paths[eCubemapSide::LEFT]);

	for (auto side : IterateEnum<eCubemapSide>())
	{
		String absolutePath = gAssetsPathConfig.GetAssetsPath(eResourceSource::ENGINE) + paths[side];
		
		int fileChannels;
		int fileWidth;
		int fileHeight;

		Images[side] = LoadImageHDR(absolutePath, &fileWidth, &fileHeight, &fileChannels);

		Width = std::max(Width, fileWidth);
		Height = std::max(Height, fileHeight);
	}

	TextureProxy = gEngine->GetRenderingDevice()->CreateCubemap(Width, Height);

	for (auto side : IterateEnum<eCubemapSide>())
	{
		TextureProxy->SetContentHDR(side, Images[side]);
	}

	for (auto side : IterateEnum<eCubemapSide>())
	{
		FreeImageHDR(Images[side]);
	}
}

CubemapResource::~CubemapResource()
{
}
