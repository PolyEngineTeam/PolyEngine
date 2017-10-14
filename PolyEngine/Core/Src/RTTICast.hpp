#pragma once

#include "Defines.hpp"
#include "Dynarray.hpp"
#include "RTTITypeInfo.hpp"
#include "RTTIProperty.hpp"


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
			// Returns dynarray of TypeInfo if class T has baseClassList, that is filled with TypeInfos of base types for type T
			template<typename C>
			static auto RetrieveImpl(int, typename std::enable_if<std::is_fundamental<C>::value>::type* = 0) -> decltype(C::baseClassList, Dynarray<TypeInfo>{}) {
				Dynarray<TypeInfo> result;
				Impl::TypeInfoFromBaseClassList<typename C::baseClassList>::Fill(result);
				return result;
			}

			// Returns dynarray of TypeInfo if class T has no baseClassList, that is empty
			template<typename C>
			static auto RetrieveImpl(...) -> decltype(Dynarray<TypeInfo>{}) {
				return Dynarray<TypeInfo>();
			}
		public:
			static Dynarray<TypeInfo> Retrieve() { return RetrieveImpl<T>(0); }
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
	return GetCheckedTypeInfo<T>();
}

// Declares type with no base class
#define RTTI_DECLARE_TYPE(T) \
public: \
RTTI_GENERATE_TYPE_INFO(T)\
virtual Poly::RTTI::TypeInfo GetTypeInfo() const { return getTypeInfoFromInstance(this); } \
typedef TYPE_LIST() baseClassList;\
RTTI_GENERATE_PROPERTY_LIST(T)


// Declares type with one base class
#define RTTI_DECLARE_TYPE_DERIVED(T,A) \
public: \
RTTI_GENERATE_TYPE_INFO(T)\
Poly::RTTI::TypeInfo GetTypeInfo() const override { return getTypeInfoFromInstance(this); } \
typedef TYPE_LIST_1(A) baseClassList;\
RTTI_GENERATE_PROPERTY_LIST(T)

// Declares type with two base classes. Disabled for now.
/*#define RTTI_DECLARE_TYPE_DERIVED2(T,A,B) \
public: \
RTTI_GENERATE_TYPE_INFO(T)\
Poly::RTTI::TypeInfo GetTypeInfo() const override { return getTypeInfoFromInstance(this); } \
typedef TYPE_LIST_2(A,B) baseClassList;\
RTTI_GENERATE_PROPERTY_LIST(T)*/
