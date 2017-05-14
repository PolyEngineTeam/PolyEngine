#pragma once

#include "Defines.hpp"
#include "Dynarray.hpp"
#include "String.hpp"

namespace Poly {

	class CORE_DLLEXPORT Path
	{
	public:
		Path();
		Path(const String& root) : Root(root) {};

		Path& Dir(const String& dir);
		Path& File(const String& file);

		bool IsAbsolute() { return !Root.GetLength(); }
		String Get();

		static const String DELIMITER;

	private:
		String Root;
		Dynarray<String> Dirs;
		String Filename;
	};
}