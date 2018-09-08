#include "ComponentIDGenerator.hpp"

namespace Poly
{
	namespace Impl
	{
		template<typename T>
		size_t ComponentIDGenerator<T>::GenerateID() noexcept
		{
			static size_t value = 0;
			return value++;
		}

		template<typename T>
		template<typename U>
		size_t ComponentIDGenerator<T>::GetComponentTypeID() noexcept
		{
			static const size_t value = GenerateID();
			return value;
		}

SILENCE_GCC_WARNING(-Wattributes, "Invalid warning about attributes")
		EXPORT_TEMPLATE template class
#if (defined(_GAME) && (defined(__GNUC__) || defined(__clang__)))

#else
			ENGINE_DLLEXPORT
#endif
			ComponentIDGenerator<struct ComponentsGroupType>;

		EXPORT_TEMPLATE template class
#if (defined(_GAME) && (defined(__GNUC__) || defined(__clang__)))

#else
			ENGINE_DLLEXPORT
#endif
			ComponentIDGenerator<struct WorldComponentsGroupType>;
	}
UNSILENCE_GCC_WARNING()
}
