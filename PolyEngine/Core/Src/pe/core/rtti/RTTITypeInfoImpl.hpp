#pragma once

#include <pe/Defines.hpp>
#include <pe/core/rtti/CustomTypeTraits.hpp>


namespace Poly {
	namespace RTTI {
		namespace Impl {
			//--------------------------------------------------------------------------
			template<typename T>
			class HasGetTypeInfoFunc {
				template <typename>
				constexpr static auto evaluate(int) -> decltype(std::declval<T>().GetTypeInfo(), bool{}) { return true; }
				
				template <typename>
				constexpr static auto evaluate(...) -> decltype(bool{}) { return false; }

			public:
				static const bool value = evaluate<typename Trait::RawType<T>::type>(0);
			};

			template <typename T>
			constexpr TypeInfo GetTypeInfoFromInstance(const T*) { return T::MetaTypeInfo::GetTypeInfo(); }
		} // namespace Impl
	} // namespace RTTI
} // namespace Poly

template <typename T>
struct AutoRegisterType {
	AutoRegisterType() {
#if defined(_WIN32)
		STATIC_ASSERTE(false, "Type not defined in rtti system!");
#else
		ASSERTE(false, "Type not defined in rtti system!");
#endif
	}
};

#define RTTI_CAT_IMPL(a,b) a##b
#define RTTI_CAT(a,b) RTTI_CAT_IMPL(a,b)

// Macro for defining new types
#define RTTI_DEFINE_TYPE(T)                       \
	template <> struct AutoRegisterType<T> { AutoRegisterType() { T::MetaTypeInfo::GetTypeInfo(); } }; \
	static const AutoRegisterType<T> RTTI_CAT(autoRegisterType, __COUNTER__); \
	RTTI_PROPERTY_MANAGER_IMPL(T)


#define RTTI_GENERATE_TYPE_INFO(T)\
	struct MetaTypeInfo { \
		static Poly::RTTI::TypeInfo GetTypeInfo() \
		{ \
			return Poly::RTTI::Impl::TypeManager::Get().RegisterOrGetType<T>(#T, Poly::RTTI::BaseClasses<T>::Retrieve()); \
		} \
	};
