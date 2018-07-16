#pragma once

#include "Defines.hpp"
#include "Collections/String.hpp"
#include "Memory/BinaryBuffer.hpp"
#include "Utils/Logger.hpp"


namespace Poly {

	//TODO add creation of folders if necessary

#if defined(__GNUC__) || defined(__clang__)
	inline int fopen_s(FILE *__restrict__ *__restrict__ streamptr, const char *__restrict__ filename, const char *__restrict__ mode) {
		FILE* f = fopen(filename, mode);
		*streamptr = f;
		return f == nullptr ? errno : 0;
	}
#endif

	//------------------------------------------------------------------------------
	SILENCE_MSVC_WARNING(4275, "Exporting stl may cause incompatibility. We use same CRT so it's ok.")
	class CORE_DLLEXPORT FileIOException : public BaseObject<>, public std::exception
	{
	public:
		FileIOException(const String& msg) : Msg(msg) {}
		const char* what() const noexcept override { return Msg.GetCStr(); }
	protected:
		String Msg;
	};
	UNSILENCE_MSVC_WARNING()

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

			char* chars = AllocateSlab(fsize + 1);

			fread(chars, fsize, 1, f); //todo: would be nice if we could read directly into the string
			fclose(f);
			chars[fsize] = '\0';

			String string(chars);
			Deallocate(chars);

			return string;
		}
		else
		{
			throw FileIOException("File open failed!");
		}
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
		{
			gConsole.LogError("File save failed: {} with path \"{}\".", strerror(errno), path);
			throw FileIOException("File save failed.");
		}
	}

	//------------------------------------------------------------------------------
	inline bool FileExists(const String& path)
	{
		FILE *f;
		fopen_s(&f, path.GetCStr(), "r");
		if (f)
		{
			fclose(f);
			return true;
		}
		else
		{
			return false;
		}
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
			fread(data->GetData(), fsize, 1, f);
			fclose(f);

			return data;
		}
		else
			throw FileIOException(path);
	}
}
