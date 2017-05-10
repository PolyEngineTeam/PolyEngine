#include "EnginePCH.hpp"

#include "ResourceManager.hpp"

#include <fstream>

using namespace Poly;


const String& Poly::GetResourcesAbsolutePath()
{
	static String PATH;

	//TODO temporary implementation. should use some JSON config file
	String defaultPath("C:\\Polyengine\\Resources");
	std::string tmpConfigPath = "tmp_config.txt";

	std::ifstream file;
	file.open(tmpConfigPath);
	if (file.good())
	{
		file.close();
		PATH = LoadTextFile(String(tmpConfigPath.c_str()));
	}
	else
	{
		file.close();
		std::ofstream ofile;
		ofile.open(tmpConfigPath);
		ofile << defaultPath;
		PATH = defaultPath;
	}
	file.close();

	return PATH;
}
