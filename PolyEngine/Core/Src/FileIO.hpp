#pragma once

#include "Defines.hpp"
#include "String.hpp"

namespace Poly {

	inline String LoadTextFile(const String& path) {
		FILE *f;
		fopen_s(&f, path.GetCStr(), "rb");
		ASSERTE(f, "Cannot open file!");
		if (f) {
			fseek(f, 0, SEEK_END);
			long fsize = ftell(f);
			fseek(f, 0, SEEK_SET);  //same as rewind(f);

			char* string = (char*)default_alloc(fsize + 1);
			fread(string, fsize, 1, f);
			fclose(f);
			string[fsize] = 0;
			return String(string);
		}
		return String();
	}
}