#include <CorePCH.hpp>

#include <Collections/StringBuilder.hpp>

using namespace Poly;

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(const char* str, const size_t length)
{
	Buffer.Reserve(Buffer.GetSize() + length);
	for (size_t i = 0; i < length; ++i)
		Append(str[i]);
	return *this;
}

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(f32 val, size_t precission)
{
	if (precission != DEFAULT_FLT_PRECISION)
	{
		CharBuffer precissionBuffer = CreateBufferFromFormat(precission, "%%." "%u" "f");
		FillBufferWithFormat(val, precissionBuffer.Data.get());
	}
	else
	{
		FillBufferWithFormat(val, "%f");
	}
	
	return *this;
}

//----------------------------------------------------------------------------
StringBuilder& StringBuilder::Append(f64 val, size_t precission)
{
	if (precission != DEFAULT_FLT_PRECISION)
	{
		CharBuffer precissionBuffer = CreateBufferFromFormat(precission, "%%." "%u" "f");
		FillBufferWithFormat(val, precissionBuffer.Data.get());
	}
	else
	{
		FillBufferWithFormat(val, "%f");
	}

	return *this;
}