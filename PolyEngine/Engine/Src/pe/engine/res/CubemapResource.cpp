#include <pe/engine/EnginePCH.hpp>

#include <pe/engine/res/CubemapResource.hpp>
#include <pe/engine/res/ResourceManager.hpp>
#include <pe/api/ecs/Scene.hpp>

namespace pe::engine::res {

CubemapResource::CubemapResource(const core::utils::EnumArray<::pe::core::storage::String, api::rendering::eCubemapSide> paths)
{
	ASSERTE(paths.GetSize() == 6, "CubemapResource::CubemapResource parths need to have 6 elements");
	core::utils::gConsole.LogInfo("CubemapResource::CubemapResource path:{}", paths[api::rendering::eCubemapSide::LEFT]);

	for (auto side : core::utils::IterateEnum<api::rendering::eCubemapSide>())
	{
		core::storage::String absolutePath = config::gAssetsPathConfig.GetAssetsPath(config::eResourceSource::ENGINE) + paths[side];
		
		int fileChannels;
		int fileWidth;
		int fileHeight;

		Images[side] = LoadImageHDR(absolutePath, &fileWidth, &fileHeight, &fileChannels);

		Width = std::max(Width, fileWidth);
		Height = std::max(Height, fileHeight);
	}

	// TextureProxy = gEngine->GetRenderingDevice()->CreateCubemap(Width, Height);

	// for (auto side : core::utils::IterateEnum<eCubemapSide>())
	// {
	// 	TextureProxy->SetContentHDR(side, Images[side]);
	// }

	// for (auto side : core::utils::IterateEnum<eCubemapSide>())
	// {
	// 	FreeImageHDR(Images[side]);
	// }
}

CubemapResource::~CubemapResource()
{
}

}