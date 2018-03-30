#include "EnginePCH.hpp"

#include "Resources/ResourceManager.hpp"
#include "Resources/MeshResource.hpp"
#include "Resources/SoundResource.hpp"

#include <fstream>

using namespace Poly;

DEFINE_RESOURCE(MeshResource, gMeshResourcesMap)
DEFINE_RESOURCE(TextureResource, gTextureResourcesMap)
DEFINE_RESOURCE(FontResource, gFontResourcesMap)
DEFINE_RESOURCE(SoundResource, gALSoundResourcesMap)

String Poly::LoadTextFileRelative(eResourceSource Source, const String & path)
{
	bool IsNotLoaded = true;

	String FileContent;
	String AbsolutePath = gAssetsPathConfig.GetAssetsPath(Source) + path;
	if (FileExists(AbsolutePath))
	{
		FileContent = LoadTextFile(AbsolutePath);
		IsNotLoaded = false;
	}


	if (IsNotLoaded)
	{
		throw FileIOException("File load failed from all sources, check config");
	}

	return FileContent;
}

void Poly::SaveTextFileRelative(eResourceSource Source, const String& path, const String& data)
{
	String absolutePath = gAssetsPathConfig.GetAssetsPath(Source) + path;
	SaveTextFile(absolutePath, data);
}