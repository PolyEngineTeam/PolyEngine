#pragma once

#include <pe/Defines.hpp>
#include <variant>

namespace pe::core::utils
{
	template <typename T, typename E>
	class Result
	{
	public:
		Result() = delete;
		Result(Result&) = delete;
		Result(Result&&) = default;
		Result& operator=(Result&) = delete;
		Result& operator=(Result&&) = default;
		Result(T okVal) : m_value(std::move(okVal)) {}
		Result(E errVal) : m_value(std::move(errVal)) {}
		
		bool IsOk() const { return m_value.index() == 0; }
		bool IsErr() const { return !IsOk(); }
		T& GetValue() { return std::get<T>(m_value); }
		E& GetError() { return std::get<E>(m_value); }
		const T& GetValue() const { return std::get<T>(m_value); }
		const E& GetError() const { return std::get<E>(m_value); }
	
	private:
		std::variant<T, E> m_value;
	};
	
	template <typename E>
	class Result<void, E> final : public BaseObjectLiteralType<>
	{
	public:
		Result() = default;
		Result(Result&) = delete;
		Result(Result&&) = default;
		Result& operator=(Result&) = delete;
		Result& operator=(Result&&) = default;
		Result(E errVal) : m_value(std::move(errVal)) {}
	
		bool IsOk() const { return m_value.has_value() == 0; }
		bool IsErr() const { return !IsOk(); }
		E& GetError() { return m_value.value(); }
		const E& GetError() const { return m_value.value(); }
	
	private:
		std::optional<E> m_value;
	};
}
