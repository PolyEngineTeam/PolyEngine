#pragma once

#include <pe/Defines.hpp>
#include <variant>

namespace pe::core::utils
{
	template <typename T, typename E>
	class Result final : public BaseObjectLiteralType<>
	{
		// if T == void then result can't have value but we can't pass 
		// void as type template parameter to variant so we need to use
		// optional instead of variant
		static constexpr bool CanHaveValue = std::negation<std::is_same<T, void>>::value;

	public:
		template <typename Dummy = void, typename Dummy2 = std::enable_if_t<CanHaveValue, Dummy>>
		Result(T okVal)
		{

		}

	private:
		const std::conditional_t<CanHaveValue, std::variant<T, E>, std::optional<E>> m_value;
	};
}
