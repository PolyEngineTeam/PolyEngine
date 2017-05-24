#pragma once

#include "Defines.hpp"
namespace Poly {

	template<typename T, typename E, size_t SIZE = static_cast<typename std::underlying_type<E>::type>(E::_COUNT)>
	class EnumArray : public BaseObject<>
	{
		using IndexType = typename std::underlying_type<E>::type;

		STATIC_ASSERTE(std::is_enum<E>::value, "Provided EnumArray key is not an enum!");
		STATIC_ASSERTE(std::is_integral<IndexType>::value, "Underlying enum value type is not integral!");
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
		T& operator[](E enumValue) {
			return const_cast<T&>(const_cast<const EnumArray&>(*this)[enumValue]); //as advised by Effective C++
		}
		const T& operator[](E enumValue) const {
			auto idx = static_cast<IndexType>(enumValue);
			HEAVY_ASSERTE(idx >= 0 && static_cast<size_t>(idx) < GetSize(), "Index out of bounds!");
			return Data[idx];
		}

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

	//------------------------------------------------------------------------------
	template<typename E>
	class EnumFlags : public BaseObjectLiteralType<>
	{
		using FlagType = typename std::underlying_type<E>::type;

		STATIC_ASSERTE(std::is_enum<E>::value, "Provided EnumFlags type is not an enum!");
		STATIC_ASSERTE(std::is_integral<FlagType>::value, "Underlying enum value type is not integral!");
	public:
		constexpr EnumFlags() {}
		constexpr EnumFlags(E enumValue) : Flags(static_cast<FlagType>(enumValue)) {}
		
		constexpr bool operator==(const EnumFlags& rhs) const { return Flags == rhs.Flags; }
		constexpr bool operator!=(const EnumFlags& rhs) const { return !(*this == rhs); }

		constexpr EnumFlags operator|(const EnumFlags& rhs) const { return EnumFlags(Flags | rhs.Flags); }
		constexpr EnumFlags operator&(const EnumFlags& rhs) const { return EnumFlags(Flags & rhs.Flags); }
		constexpr EnumFlags operator~() const { return EnumFlags(~Flags); }

		EnumFlags& operator|=(const EnumFlags& rhs) { Flags |= rhs.Flags; return *this; }
		EnumFlags& operator&=(const EnumFlags& rhs) { Flags &= rhs.Flags; return *this; }

		constexpr bool IsSet(E enumValue) const { return (*this & enumValue) == enumValue; }
		constexpr bool AnySet(E enumValue) const { return (*this & enumValue).Flags != 0; }
		constexpr bool NoneSet(E enumValue) const { return (*this & enumValue).Flags == 0; }

		constexpr explicit operator E() const { return static_cast<E>(Flags); }
		constexpr explicit operator FlagType() const { return Flags; }
	private:
		constexpr EnumFlags(FlagType flagsValue) : Flags(flagsValue) {}

		FlagType Flags = 0;
	};

	//------------------------------------------------------------------------------
	template <typename E, typename = std::enable_if_t<std::is_enum<E>::value> > constexpr E operator|(E lhs, E rhs) { return static_cast<E>(EnumFlags<E>(lhs) | EnumFlags<E>(rhs)); }
	template <typename E, typename = std::enable_if_t<std::is_enum<E>::value> > constexpr E operator&(E lhs, E rhs) { return static_cast<E>(EnumFlags<E>(lhs) & EnumFlags<E>(rhs)); }
	template <typename E, typename = std::enable_if_t<std::is_enum<E>::value> > constexpr E operator~(E rhs) { return static_cast<E>(~EnumFlags<E>(rhs)); }

	//------------------------------------------------------------------------------
	template <typename E>
	class EnumIterator : public BaseObject<>, public std::iterator<std::random_access_iterator_tag, E>
	{
		using ValueType = typename std::underlying_type<E>::type;

		STATIC_ASSERTE(std::is_enum<E>::value, "Provided EnumIterator type is not an enum!");
		STATIC_ASSERTE(std::is_integral<ValueType>::value, "Underlying enum value type is not integral!");

	public:
		bool operator==(const EnumIterator& rhs) const { return Value == rhs.Value; }
		bool operator!=(const EnumIterator& rhs) const { return !(*this == rhs); }
		bool operator<(const EnumIterator& rhs) const { return Value < rhs.Value; }
		bool operator<=(const EnumIterator& rhs) const { return Value <= rhs.Value; }
		bool operator>(const EnumIterator& rhs) const { return Value > rhs.Value; }
		bool operator>=(const EnumIterator& rhs) const { return Value >= rhs.Value; }

		E operator*() const { return static_cast<E>(Value); }

		EnumIterator& operator++() { ++Value; return *this; }
		EnumIterator operator++(int) { EnumIterator ret(Value); ++Value; return ret; }
		EnumIterator& operator--() { --Value; return *this; }
		EnumIterator operator--(int) { EnumIterator ret(Value); --Value; return ret; }

		EnumIterator operator+(ValueType val) const { return EnumIterator(Value + val); }
		EnumIterator operator-(ValueType val) const { return EnumIterator(Value - val); }
		EnumIterator& operator+=(ValueType val) { Value += val; return *this; }
		EnumIterator& operator-=(ValueType val) { Value -= val; return *this; }

		ValueType operator-(const EnumIterator& rhs) const { return Value - rhs.Value; }
	private:
		EnumIterator(E enumValue) : Value(static_cast<ValueType>(enumValue)) {}
		EnumIterator(ValueType value) : Value(value) {}

		ValueType Value;
		template<typename T> friend class EnumIteratorProxy;
	};

	//------------------------------------------------------------------------------
	template <typename E>
	class EnumIteratorProxy
	{
		STATIC_ASSERTE(std::is_enum<E>::value, "Provided EnumIteratorProxy type is not an enum!");
	public:
		EnumIterator<E> Begin() const { return EnumIterator<E>(0); }
		EnumIterator<E> End() const { return EnumIterator<E>(E::_COUNT); }
	};

	//------------------------------------------------------------------------------
	template <typename E> EnumIteratorProxy<E> IterateEnum() { return EnumIteratorProxy<E>{}; }
	template <typename E> typename Poly::EnumIterator<E> begin(const Poly::EnumIteratorProxy<E>& rhs) { return rhs.Begin(); }
	template <typename E> typename Poly::EnumIterator<E> end(const Poly::EnumIteratorProxy<E>& rhs) { return rhs.End(); }

	//------------------------------------------------------------------------------
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
//Unfortunately being compliant causes problems when using the macro in a namespace. Use _IN_POLY variant then.
#define REGISTER_ENUM_NAMES(type, ...)                                                    \
	namespace Poly { namespace Impl {                                                     \
	template<> struct EnumInfo<type> {                                                    \
		static EnumInfo<type>& Get() { static EnumInfo<type> instance; return instance; } \
		const EnumArray<const char*, type> Names = {__VA_ARGS__};                         \
	};                                                                                    \
	}} //namespace Poly::Impl

#define REGISTER_ENUM_NAMES_IN_POLY(type, ...)                                            \
	namespace Impl {                                                                      \
	template<> struct EnumInfo<type> {                                                    \
		static EnumInfo<type>& Get() { static EnumInfo<type> instance; return instance; } \
		const EnumArray<const char*, type> Names = {__VA_ARGS__};                         \
	};                                                                                    \
	} //namespace Impl
