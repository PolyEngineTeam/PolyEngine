#pragma once

#include "Defines.hpp"
namespace Poly {

	template<typename T, typename E, size_t SIZE = static_cast<typename std::underlying_type<E>::type>(E::_COUNT)>
	class EnumArray : public BaseObject<>
	{
		using IndexType = typename std::underlying_type<E>::type;

		STATIC_ASSERTE(std::is_enum<E>::value, "Provided EnumArray key is not an enum!");
		STATIC_ASSERTE(std::is_integral<IndexType>::value, "Underlying enum value type is not integral!");
		STATIC_ASSERTE(std::is_unsigned<IndexType>::value, "Underlying enum value type is not unsigned!");
		STATIC_ASSERTE(SIZE > 0, "Zero size enum array is prohibited");
	public:
		class Iterator : public BaseObject<>
		{
		public:
			bool operator==(const Iterator& rhs) const { return Data == rhs.Data && Idx == rhs.Idx; }
			bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }

			T& operator*() const { return Data[Idx]; }
			T& operator->() const { return Data[Idx]; }

			Iterator& operator++() { ++Idx; return *this; }
			Iterator operator++(int) { Iterator ret(Data, Idx); ++Idx; return ret; }
			Iterator& operator--() { --Idx; return *this; }
			Iterator operator--(int) { Iterator ret(Data, Idx); --Idx; return ret; }

			Iterator operator+(size_t val) const { return Iterator(Data, Idx + val); }
			Iterator operator-(size_t val) const { return Iterator(Data, Idx - val); }
			Iterator& operator+=(size_t val) { Idx += val; return *this; }
			Iterator& operator-=(size_t val) { Idx -= val; return *this; }
		private:
			Iterator(T* data, size_t idx) : Data(data), Idx(idx) {}

			size_t Idx = 0;
			T* Data = nullptr;
			friend class EnumArray<T, E, SIZE>;
		};

		class ConstIterator : public BaseObject<>
		{
		public:
			bool operator==(const ConstIterator& rhs) const { return Data == rhs.Data && Idx == rhs.Idx; }
			bool operator!=(const ConstIterator& rhs) const { return !(*this == rhs); }

			const T& operator*() const { return Data[Idx]; }
			const T& operator->() const { return Data[Idx]; }

			ConstIterator& operator++() { ++Idx; return *this; }
			ConstIterator operator++(int) { ConstIterator ret(Data, Idx); ++Idx; return ret; }
			ConstIterator& operator--() { --Idx; return *this; }
			ConstIterator operator--(int) { ConstIterator ret(Data, Idx); --Idx; return ret; }

			ConstIterator operator+(size_t val) const { return ConstIterator(Data, Idx + val); }
			ConstIterator operator-(size_t val) const { return ConstIterator(Data, Idx - val); }
			ConstIterator& operator+=(size_t val) { Idx += val; return *this; }
			ConstIterator& operator-=(size_t val) { Idx -= val; return *this; }
		private:
			ConstIterator(const T* data, size_t idx) : Data(data), Idx(idx) {}

			size_t Idx = 0;
			const T* Data = nullptr;
			friend class EnumArray<T, E, SIZE>;
		};

		//EnumArray() = default;
		EnumArray() : Data{} {}

		//------------------------------------------------------------------------------
		EnumArray(const std::initializer_list<T>& list)
		{
			PopulateFromInitializerList(list);
		}

		//------------------------------------------------------------------------------
		EnumArray<T, E>& operator=(const std::initializer_list<T>& list)
		{
			PopulateFromInitializerList(list);
			return *this;
		}

		//------------------------------------------------------------------------------
		bool operator==(const EnumArray<T, E, SIZE>& rhs) const
		{
			for (size_t idx = 0; idx < GetSize(); ++idx)
				if (Data[idx] != rhs.Data[idx])
					return false;

			return true;
		}

		bool operator!=(const EnumArray<T, E, SIZE>& rhs) const { return !(*this == rhs); };

		//------------------------------------------------------------------------------
		size_t GetSize() const { return SIZE; }
		T* GetData() { return Data; }
		const T* GetData() const { return Data; }

		//------------------------------------------------------------------------------
		T& operator[](E idx) { HEAVY_ASSERTE(static_cast<IndexType>(idx) < GetSize(), "Index out of bounds!"); return Data[static_cast<IndexType>(idx)]; }
		const T& operator[](E idx) const { HEAVY_ASSERTE(static_cast<IndexType>(idx) < GetSize(), "Index out of bounds!"); return Data[static_cast<IndexType>(idx)]; }

		//------------------------------------------------------------------------------
		Iterator Begin() { return Iterator(Data, 0); }
		Iterator End() { return Iterator(Data, GetSize()); }
		ConstIterator Begin() const { return ConstIterator(Data, 0); }
		ConstIterator End() const { return ConstIterator(Data, GetSize()); }


	private:
		//------------------------------------------------------------------------------
		T& operator[](size_t idx) { HEAVY_ASSERTE(idx < GetSize(), "Index out of bounds!"); return Data[idx]; }
		const T& operator[](size_t idx) const { HEAVY_ASSERTE(idx < GetSize(), "Index out of bounds!"); return Data[idx]; }

		//------------------------------------------------------------------------------
		void PopulateFromInitializerList(const std::initializer_list<T>& list)
		{
			HEAVY_ASSERTE(list.size() == GetSize(), "Initializer list size and enum array size are different!");
			size_t i = 0;
			for (const T& val : list)
				Data[i++] = val;
		}

		T Data[SIZE];
	};

	namespace Impl {
		template<typename T>
		struct EnumInfo {};
	}

	template<typename T>
	const char* GetEnumName(T val)
	{
		return Impl::EnumInfo<T>::Get().Names[val];
	}
}

//NOTE(vuko): apparently defining specializations in a namespace from global/other namespace is illegal C++ and GCC complains
//Unfortunately being compliant causes problems when using the macro in a namespace. If you run into problems, close the namespace, use the macro, then open it again
#define REGISTER_ENUM_NAMES(type, ...)                                                    \
	namespace Poly { namespace Impl {                                                     \
	template<> struct EnumInfo<type> {                                                    \
		static EnumInfo<type>& Get() { static EnumInfo<type> instance; return instance; } \
		const EnumArray<const char*, type> Names = {__VA_ARGS__};                         \
	};                                                                                    \
	}} //namespace Poly::Impl
