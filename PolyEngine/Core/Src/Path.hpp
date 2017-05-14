#pragma once

#include "Defines.hpp"
#include "Dynarray.hpp"
#include "String.hpp"

namespace Poly {

	class CORE_DLLEXPORT Path
	{
	public:
		static Path* Build();

		Path& RootAt(const String& root);
		Path& Dir(const String& dir);
		Path& File(const String& file);

		bool IsAbsolute() { return Root.GetLength(); }
		bool IsDir() { return !Filename.GetLength(); }

		String Get();

		static const String DELIMITER;

	private:
		Path() {};
		String Root;
		Dynarray<String> Dirs;
		String Filename;
	};
}