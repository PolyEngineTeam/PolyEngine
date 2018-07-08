#pragma once

#include "Defines.hpp"
#include "Collections/Dynarray.hpp"
#include "RTTITypeInfo.hpp"


namespace Poly {

	class RTTIBase;

	namespace RTTI {
		namespace Impl {
			/// <summary>Empty structure to end recurency</summary>
			struct nil_t {};

			/// <summary>Typelist entry</summary>
			template<class TypeHeld, class Next = nil_t> struct Typelist {};

			//--------------------------------------------------------------------------------------

			/// <summary>Template for filling baseClassList</summary>
			template<class> struct TypeInfoFromBaseClassList;

			template<>
			struct TypeInfoFromBaseClassList<Typelist<nil_t>>
			{
				static void Fill(Dynarray<TypeInfo>& v) { UNUSED(v); }
			};

			template<class T, class U>
			struct TypeInfoFromBaseClassList<Typelist<T, U>>
			{
				static void Fill(Dynarray<TypeInfo>& v)
				{
					v.PushBack(TypeInfo::Get<T>());
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
			template<typename C, typename std::enable_if<std::is_fundamental<C>::value>::type* = nullptr>
			static auto RetrieveImpl(int) -> decltype(C::baseClassList, Dynarray<TypeInfo>{}) {
				Dynarray<TypeInfo> result;
				Impl::TypeInfoFromBaseClassList<typename C::baseClassList>::Fill(result);
				return result;
			}

			// Returns dynarray of TypeInfo if class T has no baseClassList, that is empty
			template<typename>
			static auto RetrieveImpl(...) -> decltype(Dynarray<TypeInfo>{}) {
				return Dynarray<TypeInfo>();
			}
		public:
			static Dynarray<TypeInfo> Retrieve() { return RetrieveImpl<T>(0); }
		};

	} // namespace RTTI
}

template<typename T, typename U>
bool IsOfType(U object) {
	STATIC_ASSERTE(!std::is_pointer<T>::value, "checked type mustn't be a pointer");
	STATIC_ASSERTE(std::is_pointer<U>::value, "input type must be a pointer");
	using RawCheckedType = typename ::Poly::Trait::RawType<U>::type;
	STATIC_ASSERTE((std::is_base_of<::Poly::RTTIBase, RawCheckedType>::value), "Checked type should derive from RTTIBase!");
	// @fixme(muniu) why this assertion doesn't work?
	//STATIC_ASSERTE((std::is_same<::Poly::RTTIBase, RawCheckedType>::value || ::Poly::RTTI::Impl::HasGetTypeInfoFunc<U>::value), "no TypeInfo defined");
	if (!object)
		return false;

	Poly::RTTI::TypeInfo ti = object->GetTypeInfo();
	return ti.template isTypeDerivedFrom<T>();
}


/// <summary>Implementation of rtti_cast</summary>
template<typename T1, typename T2>
T1 rtti_cast(T2 object) {
	STATIC_ASSERTE(std::is_pointer<T1>::value, "return type must be a pointer"); // return type must be a pointer
	STATIC_ASSERTE(std::is_pointer<T2>::value, "input type must be a pointer"); // input type must be a pointer

	using ReturnType = typename std::remove_pointer<T1>::type;
	using InputType = typename std::remove_pointer<T2>::type;
	STATIC_ASSERTE((std::is_const<ReturnType>::value && std::is_const<InputType>::value) ||
		(!std::is_const<ReturnType>::value && std::is_const<InputType>::value) ||
		(!std::is_const<ReturnType>::value && !std::is_const<InputType>::value), "Return type must have const qualifier!");

	if (IsOfType<ReturnType>(object))
		return static_cast<T1>(object);
	else
		return nullptr;
}

//--------------------------------------------------------------------------------------
#define TYPE_LIST() Poly::RTTI::Impl::Typelist<Poly::RTTI::Impl::nil_t>
#define TYPE_LIST_1(A) Poly::RTTI::Impl::Typelist< A, TYPE_LIST() >
#define TYPE_LIST_2(A,B) Poly::RTTI::Impl::Typelist< A, TYPE_LIST_1(B) >