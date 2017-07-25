#pragma once

#include "Defines.hpp"
#include "Dynarray.hpp"

namespace Poly {

	template<typename T>
	size_t StrLen(const T* str) {
		size_t len = 0;
		while (str[len] != 0)
			++len;
		return len;
	}

	template<>
	inline size_t StrLen<char>(const char* str) {
		return std::strlen(str);
	}

	// TODO String class needs total refactor!
	template<typename T>
	class StringBase : BaseObject<> {
	public:
		StringBase() { Data.Resize(1); Data[0] = 0; }

		//------------------------------------------------------------------------------
		StringBase(const T* data) {
			size_t length = StrLen(data);
			Data.Resize(length + 1);
			std::memcpy(Data.GetData(), data, sizeof(T) * length);
			Data[length] = 0;
		}

		//------------------------------------------------------------------------------
		StringBase(const StringBase<T>& rhs) { *this = rhs; }
		StringBase(StringBase<T>&& rhs) { *this = rhs; }

		//------------------------------------------------------------------------------
		StringBase& operator=(const StringBase<T>& rhs) {
			Data = rhs.Data;
			return *this;
		}

		//------------------------------------------------------------------------------
		StringBase& operator=(StringBase<T>&& rhs) {
			Data = std::move(rhs.Data);
			return *this;
		}

		//------------------------------------------------------------------------------
		bool operator==(const T* str) {
			if (GetLength() != StrLen(str))
				return false;
			for (size_t k = 0; k < GetLength(); ++k)
				if (Data[k] != str[k])
					return false;
			return true;
		}

		//------------------------------------------------------------------------------
		bool operator==(const StringBase<T>& str) {
			return Data == str.Data;
		}

		//------------------------------------------------------------------------------
		bool operator<(const StringBase<T>& rhs) const {
			if (GetLength() < rhs.GetLength())
				return true;
			else if (GetLength() > rhs.GetLength())
				return false;

			for (size_t i = 0; i < GetLength(); ++i)
			{
				if(Data[i] < rhs.Data[i])
					return true;
				else if (Data[i] > rhs.Data[i])
					return false;
			}
			return false;
		}

		//------------------------------------------------------------------------------
		StringBase<T> operator+(const StringBase<T>& rhs) const {
			StringBase<T> ret;
			size_t totalLength = GetLength() + rhs.GetLength();
			ret.Data.Resize(totalLength + 1);
			memcpy(ret.Data.GetData(), Data.GetData(), sizeof(T) * GetLength());
			memcpy(ret.Data.GetData() + GetLength(), rhs.Data.GetData(), sizeof(T) * rhs.GetLength());
			ret.Data[totalLength] = 0;
			return ret;
		}

		//------------------------------------------------------------------------------
		const T& operator[](size_t idx) const { HEAVY_ASSERTE(idx <= GetLength(), "Index out of bounds!"); return Data[idx]; }
		size_t GetLength() const { return Data.GetSize() - 1; }
		const T* GetCStr() const { return Data.GetData(); }

		/*CORE_DLLEXPORT*/ friend std::ostream& operator<< (std::ostream& stream, const StringBase<T>& rhs) { return stream << rhs.GetCStr(); }

	private:
		Dynarray<T> Data;
	};

	using String = StringBase<char>;

}
