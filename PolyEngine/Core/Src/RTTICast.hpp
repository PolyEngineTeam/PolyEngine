#pragma once

#include "Defines.hpp"
#include "Dynarray.hpp"
#include "RTTITypeInfo.hpp"


namespace Poly {
	namespace RTTI {
		namespace Impl {
			/**
			* Empty structure to end recurency
			*/
			struct nil_t {};

			/**
			* Typelist entry
			*/
			template<class T, class U = nil_t>
			struct Typelist {
				typedef T head;
				typedef U tail;
			};

			/**
			* Template for assuring that class T has defined baseClassList. baseClassList contains entries of all base classes of class T.
			*/
			template <typename T>
			class HasBaseClassList_impl {
				typedef char YesType[1];
				typedef char NoType[2];

				template <typename C>
				static YesType& test(typename C::baseClassList*);

				template <typename>
				static NoType& test(...);

			public:
				static const bool value = (sizeof(YesType) == sizeof(test<T>(0)));
			};

			//--------------------------------------------------------------------------------------
			template <class T, class = void>
			struct HasBaseClassList : std::integral_constant<bool, false> {};

			template <class T>
			struct HasBaseClassList<T, typename std::enable_if<HasBaseClassList_impl<T>::value>::type > : std::integral_constant<bool, true> {};

			//--------------------------------------------------------------------------------------

			/**
			* Template for filling baseClassList
			*/
			template<class>
			struct TypeInfoFromBaseClassList;

			template<>
			struct TypeInfoFromBaseClassList<Typelist<nil_t>>
			{
				static void Fill(Dynarray<TypeInfo>& v) { UNUSED(v); }
			};

			template<class T, class U>
			struct TypeInfoFromBaseClassList<Typelist<T, U>>
			{
				static void Fill(Dynarray<TypeInfo>& v) {
					v.PushBack(MetaTypeInfo<T>::GetTypeInfo());
					TypeInfoFromBaseClassList<typename T::baseClassList>::Fill(v);
					TypeInfoFromBaseClassList<U>::Fill(v);
				}
			};

		} // namespace Impl

		  //--------------------------------------------------------------------------------------
		template<class T>
		struct BaseClasses {
		private:
			static void RetriveImpl(Dynarray<TypeInfo>& v, std::true_type) {
				Impl::TypeInfoFromBaseClassList<typename T::baseClassList>::Fill(v);
			}

			static void RetriveImpl(Dynarray<TypeInfo>& v, std::false_type) { UNUSED(v); }
		public:
			static Dynarray<TypeInfo> Retrive() {
				Dynarray<TypeInfo> result;
				RetriveImpl(result, typename Impl::HasBaseClassList<T>::type());
				return result;
			}
		};

	} // namespace RTTI
}

  /**
  * Implementation of rtti_cast
  */
template<typename T1, typename T2>
T1 rtti_cast(T2 object) {
	STATIC_ASSERTE(std::is_pointer<T1>::value, "return type must be a pointer"); // return type must be a pointer
	STATIC_ASSERTE(std::is_pointer<T2>::value, "input type must be a pointer"); // input type must be a pointer
	STATIC_ASSERTE(Poly::RTTI::Impl::HasGetTypeInfoFunc<T2>::value, "no TypeInfo defined");

	typedef typename std::remove_pointer<T1>::type ReturnType;
	typedef typename std::remove_pointer<T2>::type InputType;
	STATIC_ASSERTE((std::is_const<ReturnType>::value && std::is_const<InputType>::value) ||
		(!std::is_const<ReturnType>::value && std::is_const<InputType>::value) ||
		(!std::is_const<ReturnType>::value && !std::is_const<InputType>::value), "Return type must have const qualifier!");

	if (object && object->GetTypeInfo().template isTypeDerivedFrom<ReturnType>())
		return static_cast<T1>(object);
	else
		return nullptr;
}

template<typename T, typename U>
bool IsOfType(U object) {
	STATIC_ASSERTE(!std::is_pointer<T>::value, "checked type mustn't be a pointer");
	STATIC_ASSERTE(std::is_pointer<U>::value, "input type must be a pointer");
	STATIC_ASSERTE(Poly::RTTI::Impl::HasGetTypeInfoFunc<U>::value, "no TypeInfo defined");

	return object && object->GetTypeInfo().template isTypeDerivedFrom<T>();
}

//--------------------------------------------------------------------------------------
#define TYPE_LIST() Poly::RTTI::Impl::Typelist<Poly::RTTI::Impl::nil_t>
#define TYPE_LIST_1(A) Poly::RTTI::Impl::Typelist< A, TYPE_LIST() >
#define TYPE_LIST_2(A,B) Poly::RTTI::Impl::Typelist< A, TYPE_LIST_1(B) >

//--------------------------------------------------------------------------------------
template <typename T>
Poly::RTTI::TypeInfo getTypeInfoFromInstance(const T*) {
	return MetaTypeInfo<T>::GetTypeInfo();
}

#define RTTI_ENABLE() \
public: \
virtual Poly::RTTI::TypeInfo GetTypeInfo() const { return getTypeInfoFromInstance(this); } \
typedef TYPE_LIST() baseClassList;\
private:

#define RTTI_ENABLE_DERIVED(A) \
public: \
Poly::RTTI::TypeInfo GetTypeInfo() const override { return getTypeInfoFromInstance(this); } \
typedef TYPE_LIST_1(A) baseClassList;\
private:

#define RTTI_ENABLE_DERIVED2(A,B) \
public: \
Poly::RTTI::TypeInfo GetTypeInfo() const override { return getTypeInfoFromInstance(this); } \
typedef TYPE_LIST_2(A,B) baseClassList;\
private:
