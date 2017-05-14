#include "CorePCH.hpp"

#include "Path.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Path* Path::Build()
{
	return new Path();
}

//------------------------------------------------------------------------------
Path& Path::RootAt(const String& root)
{
	//TODO assertions
	Root = root;
	return *this;
}

//------------------------------------------------------------------------------
Path& Path::Dir(const String& dir)
{
	//TODO assertions
	ASSERTE(Filename.GetLength() == 0, "Filename already set, cannot change path");
	Dirs.PushBack(dir);
	return *this;
}

//------------------------------------------------------------------------------
Path& Path::File(const String& file)
{
	//TODO assertions
	Filename = file;
	return *this;
}

//------------------------------------------------------------------------------
String Path::Get()
{
	String dirsTmp;
	for (String s : Dirs)
	{
		if (dirsTmp.GetLength())
		{
			dirsTmp = dirsTmp + DELIMITER + s;
		}
		else
		{
			dirsTmp = s;
		}
	}

	String rootTmp;
	if (Root.GetLength())
	{
		rootTmp = Root + DELIMITER;
	}
	return rootTmp + dirsTmp + DELIMITER + Filename;
}

//------------------------------------------------------------------------------
const String Path::DELIMITER("\\");