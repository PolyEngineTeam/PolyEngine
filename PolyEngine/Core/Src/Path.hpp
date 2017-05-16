#pragma once

#include "Defines.hpp"
#include "Dynarray.hpp"
#include "String.hpp"

namespace Poly {

	class CORE_DLLEXPORT Path
	{
	public:
		static Path* Init();
		//TODO when String.split() will be ready
		static Path* FromString(const String& source);

		Path& RootAt(const String& root);
		Path& Dir(const String& dir);
		Path& File(const String& file);

		bool IsAbsolute() const { return Root.GetLength(); }
		bool IsDir() const { return !Filename.GetLength(); }

		const String& Get();
		const String& GetRoot() { return Root; }
		const String& GetFilename() { return Filename; }
		//TODO when String.split() will be ready
		//const String& GetFileExtension();

		static const String DELIMITER;

	private:
		Path() {};
		String Root;
		Dynarray<String> Dirs;
		String Filename;

		String cache;
		bool dirty = false;
	};
}