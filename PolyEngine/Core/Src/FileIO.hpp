#pragma once

#include "Defines.hpp"
#include "String.hpp"

namespace Poly {

	#if defined(__GNUC__) || defined(__clang__)
	inline int fopen_s(FILE *__restrict__ *__restrict__ streamptr, const char *__restrict__ filename, const char *__restrict__ mode) {
		FILE* f = fopen(filename, mode);
		*streamptr = f;
		return f == nullptr ? errno : 0;
	}
	#endif

	inline String LoadTextFile(const String& path) {
		FILE *f;
		fopen_s(&f, path.GetCStr(), "rb");
		ASSERTE(f, "Cannot open file!");
		if (f) {
			fseek(f, 0, SEEK_END);
			long fsize = ftell(f);
			fseek(f, 0, SEEK_SET);  //same as rewind(f);

			char* string = (char*)DefaultAlloc(fsize + 1);
			fread(string, fsize, 1, f);
			fclose(f);
			string[fsize] = 0;
			return String(string);
		}
		return String();
	}
}
