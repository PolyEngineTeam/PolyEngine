#include "EnginePCH.hpp"

#include "ResourceManager.hpp"
#include "GLMeshResource.hpp"
#include "SoundResource.hpp"

#include <fstream>

using namespace Poly;

DEFINE_RESOURCE(GLMeshResource, gGLMeshResourcesMap)
DEFINE_RESOURCE(GLTextureResource, gGLTextureResourcesMap)
DEFINE_RESOURCE(FontResource, gFontResourcesMap)
DEFINE_RESOURCE(SoundResource, gALSoundResourcesMap)


const String& Poly::GetResourcesAbsolutePath()
{
	static String PATH;
	static const String defaultPath("../Engine/Res/");
	static const String assetsPath("AssetsPath.txt");

	if (PATH.GetLength() == 0)
	{
		try
		{
			PATH = LoadTextFile(assetsPath);
		}
		catch (FileIOException)
		{
			PATH = defaultPath;
			SaveTextFile(assetsPath, defaultPath);
		}
	}

	return PATH;
}
