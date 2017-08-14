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
	bool IsNotLoaded = true;

	String FileContent;
	Dynarray<String> Paths = (Source == eResourceSource::NONE) ? Dynarray<String>{""} : gAssetsPathConfig.GetAssetsPaths(Source);
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
		throw FileIOException("File load failed from all sources, check config");
	}

	return FileContent;
}