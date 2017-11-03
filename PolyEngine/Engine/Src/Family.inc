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

	EXPIMP_TEMPLATE template class ENGINE_DLLEXPORT Family<struct ComponentsFamilyType>;
	EXPIMP_TEMPLATE template class ENGINE_DLLEXPORT Family<struct WorldComponentsFamilyType>;
}
