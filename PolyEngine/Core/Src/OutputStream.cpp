#include "CorePCH.hpp"

#include "OutputStream.hpp"

using namespace Poly;

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