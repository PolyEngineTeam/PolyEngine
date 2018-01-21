#pragma once

#include "Dynarray.hpp"
#include "Defines.hpp"
#include "String.hpp"
#include <sstream>

namespace Poly
{
	class CORE_DLLEXPORT StringBuilder
	{
	public:
		StringBuilder() = default;
		inline explicit StringBuilder(size_t preallocateSize) { Buffer.Reserve(preallocateSize); }

		inline StringBuilder& Append(char c) { Buffer.PushBack(c); return *this; }
		inline StringBuilder& Append(const char* str) { return Append(str, strlen(str)); }
		inline StringBuilder& Append(const std::string& str) { return Append(str.c_str(), str.length()); }
		inline StringBuilder& Append(const String& str) { return Append(str.GetCStr(), str.GetLength()); }
		inline StringBuilder& Append(i32 val) { FillBufferWithFormat(val, "%d"); return *this; }
		inline StringBuilder& Append(i64 val) { FillBufferWithFormat(val, "%lld"); return *this; }
		inline StringBuilder& Append(u32 val) { FillBufferWithFormat(val, "%u"); return *this; }
		inline StringBuilder& Append(u64 val) { FillBufferWithFormat(val, "%llu"); return *this; }

		inline StringBuilder& Append(long val) { return Append((i64)val); }
		inline StringBuilder& Append(unsigned long val) { return Append((u64)val); }

		StringBuilder& Append(const char* str, const size_t length);
		StringBuilder& Append(f32 val, size_t precission = DEFAULT_FLT_PRECISION);
		StringBuilder& Append(f64 val, size_t precission = DEFAULT_FLT_PRECISION);

		template <typename T>
		typename std::enable_if<!std::is_fundamental<T>::value, StringBuilder&>::type Append(const T& val)
		{
			STATIC_ASSERTE(!std::is_fundamental<T>::value, "This should never be executed for fundamental types. Fix above overloads.");
			/// @todo(muniu) Consider optimizing this bit with custom method instead of operator<<
			std::stringstream ss;
			ss << val;
			Append(ss.str().c_str(), ss.str().length());
			return *this;
		}

		
		inline StringBuilder& AppendFormat(const char* fmt) { Append(fmt); return *this; }

		template<typename T, typename... Args>
		StringBuilder& AppendFormat(const char* fmt, const T& head, Args&&... tail)
		{
			/// @todo(muniu) add size estimation
			const char* marker = strstr(fmt, "{}");
			if (marker != nullptr) // marker found
			{
				const size_t length = marker - fmt;
				Append(fmt, length);
				Append(head);
				AppendFormat(marker + 2, std::forward<Args>(tail)...);
			}
			else
			{
				Append(fmt, strlen(fmt));
			}
			return *this;
		}

		inline String GetString() const & { return String(Buffer); }
		inline String GetString() && { return StealString(); }
		inline String StealString() { return String(std::move(Buffer)); }

		inline void Clear() { Buffer.Resize(0); }
	private:
		struct CharBuffer
		{
			CharBuffer(char* data, size_t length) : Data(data), Length(length) {}
			CharBuffer(CharBuffer&& rhs) : Data(std::move(rhs.Data)), Length(rhs.Length) { rhs.Length = 0; }
			CharBuffer& operator=(CharBuffer&& rhs) { Data = std::move(rhs.Data); Length = rhs.Length; rhs.Length = 0; return *this; }

			std::unique_ptr<char> Data;
			size_t Length;
		};

		template<typename T>
		CharBuffer CreateBufferFromFormat(T val, const char* fmt)
		{
			size_t length = snprintf(nullptr, 0, fmt, val);
			char* data = new char[length + 1];
			snprintf(data, length + 1, fmt, val);
			return CharBuffer(data, length);
		}

		template<typename T>
		void FillBufferWithFormat(T val, const char* fmt)
		{
			size_t length = snprintf(nullptr, 0, fmt, val);
			size_t oldLength = Buffer.GetSize();
			Buffer.Resize(oldLength + length + 1);
			snprintf(Buffer.GetData() + oldLength, length+1, fmt, val);
			Buffer.Resize(oldLength + length);
		}

		Dynarray<char> Buffer;

		static constexpr size_t DEFAULT_FLT_PRECISION = 6;
	};
}
