#pragma once

#include "Defines.hpp"
#include "String.hpp"
#include "BinaryBuffer.hpp"

namespace Poly {

	#if defined(__GNUC__) || defined(__clang__)
	inline int fopen_s(FILE *__restrict__ *__restrict__ streamptr, const char *__restrict__ filename, const char *__restrict__ mode) {
		FILE* f = fopen(filename, mode);
		*streamptr = f;
		return f == nullptr ? errno : 0;
	}
	#endif

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT FileIOException : public BaseObject<>, public std::exception
	{
	public:
		FileIOException() {}
	};

	//------------------------------------------------------------------------------
	inline String LoadTextFile(const String& path)
	{
		FILE *f;
		fopen_s(&f, path.GetCStr(), "rb");
		if (f) 
		{
			fseek(f, 0, SEEK_END);
			long fsize = ftell(f);
			fseek(f, 0, SEEK_SET);  //same as rewind(f);

			char* string = (char*)DefaultAlloc(fsize + 1);
			fread(string, fsize, 1, f);
			fclose(f);
			string[fsize] = 0;
			return String(string);
		}
		else
			throw FileIOException();
	}

	//------------------------------------------------------------------------------
	inline void SaveTextFile(const String& path, const String& data)
	{
		FILE *f;
		fopen_s(&f, path.GetCStr(), "w");
		if (f) 
		{
			fprintf(f, "%s", data.GetCStr());
			fclose(f);
		}
		else
			throw FileIOException();
	}

	//------------------------------------------------------------------------------
	inline BinaryBuffer* LoadBinaryFile(const String& path)
	{
		FILE* f;
		fopen_s(&f, path.GetCStr(), "rb");
		if (f) 
		{
			fseek(f, 0, SEEK_END);
			long fsize = ftell(f);
			fseek(f, 0, SEEK_SET);

			BinaryBuffer* data = new BinaryBuffer(fsize);
			fread(data->GetBegin(), fsize, 1, f);
			fclose(f);

			return data;
		}
		else
			throw FileIOException();

	}
}
