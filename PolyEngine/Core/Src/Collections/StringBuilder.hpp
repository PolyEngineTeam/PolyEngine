#pragma once

#include "Dynarray.hpp"
#include "Defines.hpp"
#include "String.hpp"
#include <ostream>

namespace Poly
{
	/// Class for efficient creation of complicated strings.
	class CORE_DLLEXPORT StringBuilder : public BaseObject<>, public std::streambuf
	{
	public:
		StringBuilder() : Ostream(this) {}
		inline explicit StringBuilder(size_t preallocateSize) : StringBuilder() { Buffer.Reserve(preallocateSize); }

		/// @name Appends string representation of value to string buffer
		///	@param val Value to append
		///	@return Instance reference for chainlinking
		inline StringBuilder& Append(char val) { Buffer.PushBack(val); return *this; }
		inline StringBuilder& Append(const char* val) { return Append(val, strlen(val)); }
		inline StringBuilder& Append(const std::string& val) { return Append(val.c_str(), val.length()); }
		inline StringBuilder& Append(const String& val) { return Append(val.GetCStr(), val.GetLength()); }
		inline StringBuilder& Append(int val) { FillBufferWithFormat(val, "%d"); return *this; }
		inline StringBuilder& Append(long long val) { FillBufferWithFormat(val, "%lld"); return *this; }
		inline StringBuilder& Append(unsigned val) { FillBufferWithFormat(val, "%u"); return *this; }
		inline StringBuilder& Append(unsigned long long val) { FillBufferWithFormat(val, "%llu"); return *this; }
		inline StringBuilder& Append(long val) { return Append((long long)val); }
		inline StringBuilder& Append(unsigned long val) { return Append((unsigned long long)val); }

		StringBuilder& Append(const char* str, const size_t length);
		StringBuilder& Append(f32 val, size_t precission = DEFAULT_FLT_PRECISION);
		StringBuilder& Append(f64 val, size_t precission = DEFAULT_FLT_PRECISION);

		template <typename T>
		typename std::enable_if<!std::is_fundamental<T>::value, StringBuilder&>::type Append(const T& val)
		{
			STATIC_ASSERTE(!std::is_fundamental<T>::value, "This should never be executed for fundamental types. Fix above overloads.");
			/// @todo(muniu) Consider optimizing this bit with custom method instead of operator<<
			Ostream << val;
			return *this;
		}

		/// Fallback method for no args variant of AppendFormat
		///	@param fmt Format to use
		///	@return Instance reference for chainlinking
		inline StringBuilder& AppendFormat(const char* fmt) { return Append(fmt); }

		/// Appends formatted string to string buffer with {} used as markers for placing stringified arguments
		///	@param fmt Format to use
		/// @param head,tail Optional arguments 
		///	@return Instance reference for chainlinking
		template<typename T, typename... Args>
		StringBuilder& AppendFormat(const char* fmt, const T& head, Args&&... tail)
		{
			/// @todo(muniu) add size estimation
			const char* marker = strstr(fmt, "{}");
			if (marker != nullptr) // marker found
			{
				const size_t length = marker - fmt;
				Append(fmt, length); // append plain format string
				Append(head); // append value in place of {} marker
				// Recursive call
				AppendFormat(marker + 2, std::forward<Args>(tail)...);
			}
			else
			{
				// No marker found, just append the whole string
				Append(fmt, strlen(fmt));
			}
			return *this;
		}

		/// @name Returns copy of the string in the buffer. Data still stays in the buffer.
		///	@return String copied from string buffer
		inline String GetString() const & { return String(Buffer); }
		inline String GetString() && { return StealString(); }

		/// @name Moves data from the buffer to new string and returns it. This deletes buffer from instance.
		///	@return String with data from string buffer
		inline String StealString() { return String(std::move(Buffer)); }

		/// Clears data in the string buffer. This does not release held memory
		inline void Clear() { Buffer.Resize(0); }
	private:
		std::streamsize xsputn(const char_type* s, std::streamsize n) override final { Append(s, n); return n; }
		int_type overflow(int_type c) override final { Append((char)c); return c; }

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

		std::ostream Ostream;
		Dynarray<char> Buffer;

		static constexpr size_t DEFAULT_FLT_PRECISION = 6;
	};
}
