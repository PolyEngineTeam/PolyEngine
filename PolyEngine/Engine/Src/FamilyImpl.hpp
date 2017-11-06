#include "Family.hpp"

namespace Poly
{
	template<typename T>
	size_t Family<T>::Identifier() noexcept
	{
		static size_t value = 0;
		return value++;
	}

	template<typename T>
	template<typename U>
	size_t Family<T>::GetComponentTypeID() noexcept
	{
		static const size_t value = Identifier();
		return value;
	}

	EXPORT_TEMPLATE template class
#if (defined(_GAME) && (defined(__GNUC__) || defined(__clang__)))

#else
	ENGINE_DLLEXPORT
#endif
	Family<struct ComponentsFamilyType>;

	EXPORT_TEMPLATE template class
#if (defined(_GAME) && (defined(__GNUC__) || defined(__clang__)))

#else
	ENGINE_DLLEXPORT
#endif
	Family<struct WorldComponentsFamilyType>;
}
