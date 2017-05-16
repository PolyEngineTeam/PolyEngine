#include "CorePCH.hpp"

#include "Path.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Path* Path::Init()
{
	return new Path();
}

//------------------------------------------------------------------------------
Path& Path::RootAt(const String& root)
{
	//TODO check if given root is valid with given system?
	//eg. check if root is 'X:' when on windows
	ASSERTE(Dirs.GetSize() == 0, "Directories already set, cannot change root");
	ASSERTE(Filename.GetLength() == 0, "Filename already set, cannot change root");
	Root = root;
	dirty = true;
	return *this;
}

//------------------------------------------------------------------------------
Path& Path::Dir(const String& dir)
{
	ASSERTE(Filename.GetLength() == 0, "Filename already set, cannot change path");
	Dirs.PushBack(dir);
	dirty = true;
	return *this;
}

//------------------------------------------------------------------------------
Path& Path::File(const String& file)
{
	Filename = file;
	dirty = true;
	return *this;
}

//------------------------------------------------------------------------------
const String& Path::Get()
{
	if (dirty)
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

		dirty = false;
		cache = rootTmp + dirsTmp + DELIMITER + Filename;
	}

	return cache;
}

//------------------------------------------------------------------------------
#ifdef _WIN32
	const String Path::DELIMITER("\\");
#elif defined(__linux__)
	const String Path::DELIMITER("/");
#endif