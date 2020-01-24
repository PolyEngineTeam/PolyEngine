#pragma once

#include <pe/Defines.hpp>
#include <variant>

namespace pe::core::utils
{
	//------------------------------------------------------------------------------
	template <typename T>
	class Ok
	{
	public:
		Ok() = delete;
		Ok(Ok&) = delete;
		Ok(Ok&&) = default;
		Ok& operator=(Ok&) = delete;
		Ok& operator=(Ok&&) = delete;

		explicit constexpr Ok(T val) : m_value(std::move(val)) {}
		constexpr T&& get() && { return std::move(m_value); }

	private:
		T m_value;
	};

	//------------------------------------------------------------------------------
	template <typename E>
	class Err
	{
	public:
		Err() = delete;
		Err(Err&) = delete;
		Err(Err&&) = default;
		Err& operator=(Err&) = delete;
		Err& operator=(Err&&) = delete;

		explicit constexpr Err(E val) : m_value(std::move(val)) {}
		constexpr E&& get() && { return std::move(m_value); }

	private:
		E m_value;
	};

	//------------------------------------------------------------------------------
	// Result is an utility class that can be used when function can
	// return result or error code. Its main purpose is to enable 
	// writing exception free code. You can't call getError when Result
	// has valid success value and you can't call getValue when Result
	// has error value but for 
	// @param T - type of correct value (value returned by method if 
	//		its execution was successful
	// @param E - type of error
	template <typename T, typename E>
	class Result
	{
	public:
		// You can't create empty Result. Result must always store 
		// valid value or error value 
		Result() = delete;

		// We're ok with default implementations. Some of these ctors 
		// will be implicitly deleted if T or E aren't copy 
		// constructible or copyable and so on.
		Result(Result&) = default;
		Result(Result&&) = default;
		Result& operator=(Result&) = default;
		Result& operator=(Result&&) = default;

		// Ctor taking ok value. Object created with this ctor will
		// return true from isOk and false from isErr.
		// @param okVal - value for success result
		constexpr Result(Ok<T> okVal) : m_value(std::move(okVal).get()) {}

		// Ctor taking error value. Object created with this ctor will
		// return false from isOk and true from isErr.
		// @param errVal - value for error/invalid result
		constexpr Result(Err<E> errVal) : m_value(std::move(errVal).get()) {}
		
		// @returns true if result stores valid value, false if stores 
		//		error value
		constexpr bool isOk() const { return m_value.index() == 0; }

		// @returns false if result stores valid value, true if stores 
		//		error value
		constexpr bool isErr() const { return !isOk(); }
		
		// @returns if is ok returns valid value, crashes otherwise
		constexpr T& getValue() { return std::get<T>(m_value); }

		// @returns valid value if has valid value or given alternative value if has error
		constexpr T getValueOr(T alt) &&
		{ 
			if (isOk()) 
				return std::move(getValue()); 
			else 
				return alt;
		
		}

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
