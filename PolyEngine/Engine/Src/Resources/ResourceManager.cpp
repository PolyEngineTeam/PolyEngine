#include <EnginePCH.hpp>

#include <Resources/ResourceManager.hpp>
#include <Resources/MeshResource.hpp>
#include <Resources/SoundResource.hpp>

SILENCE_CLANG_WARNING(-Wparentheses-equality, "Surpressing clang warnings in stb_image") //@fixme(celeborth) if put in PCH it throws violation of ODR linker error
SILENCE_GCC_WARNING(-Wimplicit-fallthrough=, "Surpressing clang warnings in stb_image")
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
UNSILENCE_GCC_WARNING()
UNSILENCE_CLANG_WARNING()

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

float* Poly::LoadImageHDR(const String& path, int* width, int* height, int* fileChannels)
{
	if (!stbi_is_hdr(path.GetCStr()))
	{
		gConsole.LogWarning("Poly::LoadImageHDR LDR file spotted, scalling to HDR and gamma change may appear!");
	}

	stbi_set_flip_vertically_on_load(true);
	float *data = stbi_loadf(path.GetCStr(), width, height, fileChannels, 0);
	if (!data)
	{
		gConsole.LogInfo("Poly::LoadImageHDR Failed to load: {}, reason: {}", path, stbi_failure_reason());
	}
	
	return data;
}

void Poly::FreeImageHDR(float* data) 
{
	stbi_image_free(data);
}

unsigned char* Poly::LoadImage(const String& path, int* width, int* height, int* fileChannels, int desiredChannels)
{
	if (stbi_is_hdr(path.GetCStr()))
	{
		gConsole.LogWarning("Poly::LoadImageHDR HDR file spotted, scalling to LDR and gamma change may appear!");
	}

	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path.GetCStr(), width, height, fileChannels, desiredChannels);
	if (!data)
	{
	 gConsole.LogInfo("Poly::LoadImage Failed to load: {}, reason: {}", path, stbi_failure_reason());
	}
	
	return data;
}

void Poly::FreeImage(unsigned char* data)
{
	stbi_image_free(data);
}
