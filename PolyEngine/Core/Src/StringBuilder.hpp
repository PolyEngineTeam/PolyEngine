#pragma once

#include "Dynarray.hpp"
#include "Defines.hpp"
#include "String.hpp"

namespace Poly
{
	class CORE_DLLEXPORT StringBuilder
	{
	public:
		StringBuilder() = default;
		StringBuilder(size_t preallocateSize) { Buffer.Reserve(preallocateSize); }

		StringBuilder& Append(char c);
		StringBuilder& Append(const char* str);
		StringBuilder& Append(const char* str, const size_t length);
		StringBuilder& Append(const String& str);
		StringBuilder& Append(i32 val);
		StringBuilder& Append(i64 val);
		StringBuilder& Append(u32 val);
		StringBuilder& Append(u64 val);
		StringBuilder& Append(f32 val, size_t precission = 6);
		StringBuilder& Append(f64 val, size_t precission = 6);

		String GetString() const { return String(Buffer); }
		String StealString() { return String(std::move(Buffer)); }

		void Clear() { Buffer.Resize(0); }
	private:
		struct CharBuffer
		{
			CharBuffer(char* data, size_t length) : Data(data), Length(length) {}
			CharBuffer(CharBuffer&& rhs) : Data(std::move(rhs.Data)), Length(rhs.Length) { rhs.Length = 0; }
			CharBuffer& operator=(CharBuffer&& rhs) { Data = std::move(rhs.Data); Length = rhs.Length; rhs.Length = 0; }

			std::unique_ptr<char> Data;
			size_t Length;
		};

		template<typename T>
		CharBuffer CreateBufferFromVal(T val, const char* spec)
		{
			size_t length = snprintf(nullptr, 0, spec, val);
			char* data = new char[length + 1];
			snprintf(data, length + 1, spec, val);
			return CharBuffer(data, length);
		}

		Dynarray<char> Buffer;
	};
}
