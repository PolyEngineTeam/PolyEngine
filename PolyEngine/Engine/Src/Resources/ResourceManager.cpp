#include "EnginePCH.hpp"

#include "Resources/ResourceManager.hpp"
#include "Resources/MeshResource.hpp"
#include "Resources/SoundResource.hpp"

#include <fstream>

// #include "SOIL/SOIL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

using namespace Poly;

DEFINE_RESOURCE(MeshResource, gMeshResourcesMap)
DEFINE_RESOURCE(TextureResource, gTextureResourcesMap)
DEFINE_RESOURCE(FontResource, gFontResourcesMap)
DEFINE_RESOURCE(SoundResource, gALSoundResourcesMap)

String Poly::EvaluateFullResourcePath(eResourceSource Source, const String& path)
{
	return gAssetsPathConfig.GetAssetsPath(Source) + path;
}

String Poly::LoadTextFileRelative(eResourceSource Source, const String & path)
{
	bool IsNotLoaded = true;

	String FileContent;
	String AbsolutePath = EvaluateFullResourcePath(Source, path);
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
	String absolutePath = EvaluateFullResourcePath(Source, path);
	SaveTextFile(absolutePath, data);
}

float* Poly::LoadImage(const String& path, int* width, int* height, int* channels)
{
	stbi_set_flip_vertically_on_load(true);
	float *data = stbi_loadf(path.GetCStr(), width, height, channels, 0);
	if (!data)
	{
		gConsole.LogInfo("Poly::LoadImageHDR Failed to load: {}", path);
	}
	return data;
}

void Poly::FreeImage(float* data) 
{
	stbi_image_free(data);
}