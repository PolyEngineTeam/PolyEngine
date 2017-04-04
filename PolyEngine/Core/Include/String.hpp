#pragma once

#include <Core.hpp>

namespace Poly {

	template<typename T>
	size_t StrLen(const T* str) {
		size_t len = 0;
		while (str[len] != 0)
			++len;
		return len;
	}

	//TODO rewrite this with dynarray
	template<typename T>
	class StringBase : BaseObject<> {
	public:
		StringBase() = default;

		//------------------------------------------------------------------------------
		StringBase(const T* data) {
			Length = StrLen(data);
			Data = (T*)default_alloc(sizeof(T) * (Length + 1));
			memcpy(Data, data, sizeof(T) * Length);
			Data[Length] = 0;
		}

		//------------------------------------------------------------------------------
		StringBase(const StringBase<T>& rhs) { *this = rhs; }
		StringBase(StringBase<T>&& rhs) { *this = rhs; }

		//------------------------------------------------------------------------------
		StringBase& operator=(const StringBase<T>& rhs) {
			Length = rhs.GetLength();
			Data = (T*)default_alloc(sizeof(T) * (Length + 1));
			memcpy(Data, rhs.Data, sizeof(T) * Length);
			Data[Length] = 0;
		}

		//------------------------------------------------------------------------------
		StringBase& operator=(StringBase<T>&& rhs) {
			Data = rhs.Data;
			rhs.Data = nullptr;
			Length = rhs.Length;
			rhs.Length = 0;
		}

		//------------------------------------------------------------------------------
		virtual ~StringBase() {
			if (Data)
				default_free(Data);
		}

		//------------------------------------------------------------------------------
		bool operator==(const T* str) {
			if (Length != StrLen(str))
				return false;
			for (size_t k = 0; k < Length; ++k)
				if (Data[k] != str[k])
					return false;
			return true;
		}

		//------------------------------------------------------------------------------
		bool operator==(const StringBase<T>& str) {
			if (Length != str.Length)
				return false;
			for (size_t k = 0; k < Length; ++k)
				if (Data[k] != str.Data[k])
					return false;
			return true;
		}

		//------------------------------------------------------------------------------
		StringBase<T> operator+(const StringBase<T> rhs) const {
			StringBase<T> ret;
			ret.Length = Length + rhs.Length;
			ret.Data = (T*)default_alloc(sizeof(T) * (Length + 1));
			memcpy(ret.Data, Data, sizeof(T) * Length);
			memcpy(ret.Data + rhs.Length, rhs.Data, sizeof(T) * rhs.Length);
			ret.Data[Length] = 0;
			return ret;
		}

		//------------------------------------------------------------------------------
		const T& operator[](int idx) const { HEAVY_ASSERTE(idx <= Length, "Index out of bounds!"); return Data[idx]; }
		size_t GetLength() const { return Length; }
		const T* GetCStr() const { return Data; }

		DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const StringBase<T>& rhs) { return stream << rhs.GetCStr(); }

	private:
		T* Data = nullptr;
		size_t Length = 0;
	};

	typedef StringBase<char> String;

}