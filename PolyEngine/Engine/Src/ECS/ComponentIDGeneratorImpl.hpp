#include <ECS/ComponentIDGenerator.hpp>

namespace Poly
{
	namespace Impl
	{
		template<typename U>
		size_t ComponentIDGenerator::GetComponentTypeID() noexcept
		{
			static const size_t value = GenerateID();
			return value;
		}
	}
}
