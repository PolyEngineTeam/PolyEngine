#pragma once

#include "Dynarray.hpp"
#include "Defines.hpp"

namespace Poly {

	size_t StrLen(const char* str);

	class CORE_DLLEXPORT String : BaseObject<> {
	public:
		String() {};
		String(const char* data);
		String(const std::string& data);
		String(const String& rhs);
		String(String&& rhs);

		static String* From(int var);
		static String* From(float var);
		static String* From(double var);
		static String* From(char var); //??
		static String* From(const char* var);
		static String* From(const std::string& var);

		bool Contains(const String& var) const;
		String& ToLower();
		String& ToUpper();
		bool IsEmpty() const;
		String& Replace(char a, char b) const;
		String& Replace(const String& a, const String& b) const;
		String* Split(String& splitBy) const;
		static String& Join(const String* vars, size_t size, const String& separator);
		bool StartsWith(char var) const;
		bool EndsWith(char var) const;
		String& Substring(int end) const;
		String& Substring(int start, int end) const;
		String& GetTrimmed() const;


		String& operator=(const String& rhs);
		String& operator=(String&& rhs);
		bool operator==(const char* str);
		bool operator==(const String& str);
		bool operator<(const String& rhs) const;
		String operator+(const String& rhs) const;
		const char& operator[](int idx) const;
		size_t GetLength() const { return Data.GetSize() - 1; }
		const char* GetCStr() const { return Data.GetData(); }

		/*CORE_DLLEXPORT*/ friend std::ostream& operator<< (std::ostream& stream, const String& rhs) { return stream << rhs.GetCStr(); }

	private:
		Dynarray<char> Data;
	};
}
