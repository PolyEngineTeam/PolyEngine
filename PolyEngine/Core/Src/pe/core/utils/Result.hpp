#pragma once

#include <pe/Defines.hpp>
#include <variant>

namespace pe::core::utils
{
	//------------------------------------------------------------------------------
	template <typename T, typename E>
	class Result
	{
	public:
		Result() = delete;
		Result(Result&) = default;
		Result(Result&&) = default;
		Result& operator=(Result&) = default;
		Result& operator=(Result&&) = default;
		Result(T okVal) : m_value(std::move(okVal)) {}
		Result(E errVal) : m_value(std::move(errVal)) {}
		
		bool isOk() const { return m_value.index() == 0; }
		bool isErr() const { return !isOk(); }
		
		T& getValue() { return std::get<T>(m_value); }
		E& getError() { return std::get<E>(m_value); }
		const T& getValue() const { return std::get<T>(m_value); }
		const E& getError() const { return std::get<E>(m_value); }

		template <typename U>
		Result<U, E> first(Result<U, E> res)
		{
			if (isOk())
				return res;
			else
				return getError();
		}

		template <typename F>
		Result<T, F> last(Result<T, F> res)
		{
			if (isOk())
				return getValue();
			else
				return res;
		}

	private:
		std::variant<T, E> m_value;
	};

	//------------------------------------------------------------------------------
	template <typename E>
	class Result<void, E> final : public BaseObjectLiteralType<>
	{
	public:
		Result() = default;
		Result(Result&) = default;
		Result(Result&&) = default;
		Result& operator=(Result&) = default;
		Result& operator=(Result&&) = default;
		Result(E errVal) : m_value(std::move(errVal)) {}

		bool isOk() const { return m_value.has_value() == false; }
		bool isErr() const { return !isOk(); }

		E& getError() { return m_value.value(); }
		const E& getError() const { return m_value.value(); }

		template <typename U>
		Result<U, E> first(Result<U, E> res)
		{
			if (isOk())
				return res;
			else
				return getError();
		}

		template <typename F>
		Result<void, F> last(Result<void, F> res)
		{
			if (isOk())
				return Result<void, F>();
			else
				return res;
		}

	private:
		std::optional<E> m_value;
	};

	namespace result
	{
		template <typename T, typename E>
		Result<T, E> join(Result<Result<T, E>, E> res)
		{
			if (res.isOk())
				return res.getValue();
			else
				return res.getError();
		}

		template <typename E>
		Result<void, E> join(Result<Result<void, E>, E> res)
		{
			if (res.isOk())
				return Result<void, E>();
			else
				return res.getError();
		}

		template <typename T1, typename T2, typename E>
		Result<T2, E> bind(Result<T1, E> res, Result<T2, E>(*func)(T1))
		{
			if (res.isOk())
				return func(res.getValue());
			else
				return res.getError();
		}

		template <typename E, typename Res, typename... Args>
		auto lift(Res(*srcFunc)(Args...))
		{
			return [srcFunc] (Result<Args, E>... args)
			{
				return srcFunc(args.getValue()...);
			};
		}
	}
}
