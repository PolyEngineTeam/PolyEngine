#include "EnginePCH.hpp"

#include "ResourceManager.hpp"
#include "MeshResource.hpp"
#include "SoundResource.hpp"


#include <fstream>

using namespace Poly;

DEFINE_RESOURCE(MeshResource, gMeshResourcesMap)
DEFINE_RESOURCE(TextureResource, gTextureResourcesMap)
DEFINE_RESOURCE(FontResource, gFontResourcesMap)
DEFINE_RESOURCE(SoundResource, gALSoundResourcesMap)

ENGINE_DLLEXPORT String Poly::LoadTextFileRelative(eResourceSource Source, const String & path)
{
	static const String DEFAULT_PATH("../../Engine/Res/");
	bool IsNotLoaded = true;

	// TODO: move to very first start block in program
	if (!gCoreConfig.IsLoadedFromFile())
	{
		gCoreConfig.ReloadFromFile();
	}

	String FileContent;
	Dynarray<String> Paths = gCoreConfig.GetAssetsPaths(eResourceSource::ENGINE);
	for (int i = 0; i < Paths.GetSize() && IsNotLoaded; ++i)
	{
		String AbsolutePath = Paths[i] + path;
		if (FileExists(AbsolutePath))
		{
			FileContent = LoadTextFile(AbsolutePath);
			IsNotLoaded = false;
		}
	}

	if (IsNotLoaded)
	{
		String AbsolutePath = DEFAULT_PATH + path;
		if (FileExists(AbsolutePath))
		{
			FileContent = LoadTextFile(AbsolutePath);
			IsNotLoaded = false;
		}
	}

	if (IsNotLoaded)
	{
		throw FileIOException("File load failed from all sources, check config");
	}

	return FileContent;
}