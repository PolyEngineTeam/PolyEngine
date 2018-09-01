#include "CorePCH.hpp"

#include"OutputStream.hpp"
#include "Utils/FileIO.hpp"

using namespace Poly;

std::streamsize OutputStream::xsputn(const char_type* s, std::streamsize n)
{
	UNUSED(n);
	std::string str(s);
	size_t len = str.length();
	Append(s);
	return len;
}

std::streambuf::int_type OutputStream::overflow(int_type c)
{
	char tab[2];
	tab[0] = c;
	tab[1] = '\0';
	Append(tab);
	return c;
}

FileOutputStream& FileOutputStream::operator=(FileOutputStream&& rhs)
{
	EnsureFileClosed();
	FileHandle = rhs.FileHandle;
	rhs.FileHandle = nullptr;
	return *this;
}

FileOutputStream::FileOutputStream(FileOutputStream&& rhs)
{
	*this = std::move(rhs);
}

void FileOutputStream::EnsureFileClosed() {
	if (FileHandle)
	{
		fclose(FileHandle);
		FileHandle = nullptr;
	}
}

void FileOutputStream::Append(const char* data)
{
	std::string str(data);
	size_t len = str.length();
	fwrite(data, sizeof(char), len, FileHandle);
}

FileOutputStream::FileOutputStream(const char* name)
{
#if defined(_WIN32)
	fopen_s(&FileHandle, name, "w");
#else
	fopen_s(&FileHandle, name, "w");
#endif
	ASSERTE(FileHandle != nullptr, "Could not open the log file!");
	if (FileHandle)
		setvbuf(FileHandle, NULL, _IONBF, 0);
}