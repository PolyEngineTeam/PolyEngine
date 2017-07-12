#pragma once

#include "Defines.hpp"
#include "Dynarray.hpp"

namespace Poly {


	class String : BaseObject<> {
	public:
		String();
		String(const char* data);
		String(const String& rhs);
		String(String&& rhs);

		static String& From(int a);

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
