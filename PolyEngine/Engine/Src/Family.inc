#include "Family.hpp"

namespace Poly
{
	template<typename T>
	size_t Family<T>::identifier()
	{
		static size_t value = 0;
		return value++;
	}

	template<typename T>
	template<typename U>
	size_t Family<T>::type_id()
	{
		static const size_t value = identifier();
		return value;
	}

	EXPIMP_TEMPLATE template class
#if (defined(_GAME) && (defined(__GNUC__) || defined(__clang__)))

#else
	ENGINE_DLLEXPORT
#endif
	Family<struct ComponentsFamilyType>;

	EXPIMP_TEMPLATE template class
#if (defined(_GAME) && (defined(__GNUC__) || defined(__clang__)))

#else
	ENGINE_DLLEXPORT
#endif
	Family<struct WorldComponentsFamilyType>;
}
