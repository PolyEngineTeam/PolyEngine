#include "CorePCH.hpp"

#include "StringBuilder.hpp"
#include <iomanip>
#include <sstream>

using namespace Poly;

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(char c) 
{ 
	Buffer.PushBack(c); 
	return *this; 
}

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(const char* str)
{
	Append(str, strlen(str));
	return *this;
}

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(const char* str, const size_t length)
{
	Buffer.Reserve(Buffer.GetSize() + length);
	for (size_t i = 0; i < length; ++i)
		Append(str[i]);
	return *this;
}

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(const String& str)
{
	Append(str.GetCStr(), str.GetLength());
	return *this;
}

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(i32 val)
{
	CharBuffer buffer = CreateBufferFromVal(val, "%d");
	Append(buffer.Data.get(), buffer.Length);
	return *this;
}

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(i64 val)
{
	CharBuffer buffer = CreateBufferFromVal(val, "%lld");
	Append(buffer.Data.get(), buffer.Length);
	return *this;
}

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(u32 val)
{
	CharBuffer buffer = CreateBufferFromVal(val, "%u");
	Append(buffer.Data.get(), buffer.Length);
	return *this;
}

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(u64 val)
{
	CharBuffer buffer = CreateBufferFromVal(val, "%llu");
	Append(buffer.Data.get(), buffer.Length);
	return *this;
}

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(f32 val, size_t precission)
{
	CharBuffer precissionBuffer = CreateBufferFromVal(precission, "%%." "%u" "f");
	CharBuffer buffer = CreateBufferFromVal(val, precissionBuffer.Data.get());
	Append(buffer.Data.get(), buffer.Length);
	return *this;
}

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(f64 val, size_t precission)
{
	CharBuffer precissionBuffer = CreateBufferFromVal(precission, "%%." "%u" "f");
	CharBuffer buffer = CreateBufferFromVal(val, precissionBuffer.Data.get());
	Append(buffer.Data.get(), buffer.Length);
	return *this;
}