#include "EnginePCH.hpp"

#include "ResourceManager.hpp"

#include <fstream>

using namespace Poly;


const String& Poly::GetResourcesAbsolutePath()
{
	static String PATH;
	static String defaultPath("../Engine/Res/");
	static String assetsPath("AssetsPath.txt");

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
