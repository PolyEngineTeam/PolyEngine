#include "CorePCH.hpp"

#include "Path.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Path& Path::Dir(const String& dir)
{
	ASSERTE(Filename.GetLength() == 0, "Filename already set, cannot change path");
	Dirs.PushBack(dir);
	return *this;
}

//------------------------------------------------------------------------------
Path& Path::File(const String& file)
{
	Filename = file;
	return *this;
}

//------------------------------------------------------------------------------
String Path::Get()
{
	String dirs;
	for (String s : Dirs)
	{
		if (dirs.GetLength() == 0)
		{
			dirs = s;
		}
		else
		{
			dirs = dirs + DELIMITER + s;
		}
	}

	return Root + DELIMITER + dirs + DELIMITER + Filename;
}

//------------------------------------------------------------------------------
const String Path::DELIMITER("\\");