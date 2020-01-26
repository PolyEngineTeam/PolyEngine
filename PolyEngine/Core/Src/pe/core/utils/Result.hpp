#pragma once

#include <pe/Defines.hpp>
#include <variant>

namespace pe::core::utils
{
	//------------------------------------------------------------------------------
	// Utility class to use Result class safely and conveniently. Allows 
	// user to explicitly convert to valid value and assign it to Result 
	// instance without explicitly creating new instance of Result.
	// @tparam T - valid value type, must match value type of Result 
	//		instance to which error should be assigned
	template <typename T = void>
	class Ok
	{
	public:
		Ok() = delete;
		Ok(Ok&) = delete;
		constexpr Ok(Ok&&) = default;
		Ok& operator=(Ok&) = delete;
		Ok& operator=(Ok&&) = delete;

		// This is the only ctor of this class. It just takes value and 
		// stores it until someone (Result ctor) takes this value out of it.
		// @param val - valid value that should be moved into Result
		explicit constexpr Ok(T val) : m_value(std::move(val)) {}


		// @returns stored value, after this operation instance of this class
		//		doesn't store anything and shouldn't be used anymore.
		constexpr T&& get() && { return std::move(m_value); }

	private:
		T m_value;
	};

	//------------------------------------------------------------------------------
	// Specialization of Ok utility class to allow usere explicitly set 
	// Result value which valid value type (T) is void to ok value.
	template <>
	class Ok<void>
	{
	public:
		constexpr Ok() = default;
		Ok(Ok&) = delete;
		constexpr Ok(Ok&&) = default;
		Ok& operator=(Ok&) = delete;
		Ok& operator=(Ok&&) = delete;
	};

	//------------------------------------------------------------------------------
	// Utility class to use Result class safely and conveniently. Allows 
	// user to explicitly convert to error value and assign it to Result 
	// instance without explicitly creating new instance of Result.
	// @tparam E - error type, must match error type of Result instance
	//		to which error should be assigned
	template <typename E = const char*>
	class Err
	{
	public:
		Err() = delete;
		Err(Err&) = delete;
		constexpr Err(Err&&) = default;
		Err& operator=(Err&) = delete;
		Err& operator=(Err&&) = delete;

		// This is the only ctor of this class. It just takes value and 
		// stores it until someone (Result ctor) takes this value out of it.
		// @param val - error value that should be moved into result
		explicit constexpr Err(E val) : m_value(std::move(val)) {}

		// @returns stored value, after this operation instance of this class
		//		doesn't store anything and shouldn't be used anymore.
		constexpr E&& get() && { return std::move(m_value); }

	private:
		E m_value;
	};

	//------------------------------------------------------------------------------
	// const char* is treated as default error msg. If you don't know yet 
	// which errors will you retutn or you don't care about error being 
	// descriptive you can use this feature
	template <>
	class Err<const char*>
	{
	public:
		static constexpr const char* DEFAULT_ERROR_MESSAGE = "Unknown error";

		// You can create default versiton of error with error message 
		// unchanged.
		constexpr Err() = default;
		Err(Err&) = delete;
		constexpr Err(Err&&) = default;
		Err& operator=(Err&) = delete;
		Err& operator=(Err&&) = delete;

		// You can store error message in this class until someone (Result 
		// ctor) takes this value out of it.
		// @param val - error message
		explicit constexpr Err(const char* val) : m_value(val) {}

		// @returns stored error message
		constexpr const char* get() && { return m_value; }

	private:
		const char* m_value = DEFAULT_ERROR_MESSAGE;
	};

	//------------------------------------------------------------------------------
	// Result is an utility class that can be used when function can
	// return result or error code. Its main purpose is to enable 
	// writing exception free code. You can't call getError when Result
	// has valid success value and you can't call getValue when Result
	// has error value but for 
	// @tparam T - type of correct value (value returned by method if 
	//		its execution was successful
	// @tparam E - type of error
	template <typename T, typename E = const char*>
	class Result
	{
	public:
		// You can't create empty Result. Result must always store valid 
		// value or error value 
		Result() = delete;

		// We're ok with default implementations. Some of these ctors will be
		// implicitly deleted if T or E aren't copy constructible or copyable
		// and so on.
		constexpr Result(Result&) = default;
		constexpr Result(Result&&) = default;
		constexpr Result& operator=(Result&) = default;
		constexpr Result& operator=(Result&&) = default;

		// Ctor taking ok value. Object created with this ctor will return 
		// true from isOk and false from isErr.
		// @param okVal - value for success result
		constexpr Result(Ok<T> okVal) : m_value(std::move(okVal).get()) {}

		// Ctor taking error value. Object created with this ctor will return
		// false from isOk and true from isErr.
		// @param errVal - value for error/invalid result
		constexpr Result(Err<E> errVal) : m_value(std::move(errVal).get()) {}

		// Ctor taking ok value. Object created with this ctor will return 
		// true from isOk and false from isErr. Using this ctor rather than
		// the one taing Ok<T> will save you one move operation.
		// @param okVal - value for success result
		explicit constexpr Result(T okVal) : m_value(std::move(okVal)) {}

		// Ctor taking error value. Object created with this ctor will return
		// false from isOk and true from isErr. Using this ctor rather than
		// the one taing Err<T> will save you one move operation.
		// @param errVal - value for error/invalid result
		explicit constexpr Result(E errVal) : m_value(std::move(errVal)) {}
		
		// @returns true if result stores valid value, false if stores error
		//		value
		constexpr bool isOk() const { return m_value.index() == 0; }

		// @returns false if result stores valid value, true if stores error
		//		value
		constexpr bool isErr() const { return !isOk(); }
		
		// @returns valid value if has valid value, crashes otherwise :(
		constexpr T& value() { return std::get<T>(m_value); }

		// @returns valid value if has valid value, crashes otherwise :(
		constexpr const T& value() const { return std::get<T>(m_value); }

		// Version of valueOr which can be called for almost every instance
		// of Result except from instance where correct value type (T) is not
		// copy constructible. 
		// @tparam U - type of alternative value to return. U must be 
		//		convertible to T
		// @param alt - alternative value to return if this doesn't contain 
		//		valid value
		// @returns valid value if has valid value or given alternative value
		//		if has error
		template <typename U>
		constexpr T valueOr(U&& alt) const&
		{
			if (isOk())
				return value();
			else
				return static_cast<T>(std::forward<U>(alt));

		}

		// Verstion of valueOr which is called for instances of Result where 
		// stored valid value type (T) isn't copy constructible but instead 
		// has to be move constructible.
		// @tparam U - type of alternative value to return. U must be 
		//		convertible to T
		// @param alt - alternative value to return if this doesn't contain 
		//		valid value
		// @returns valid value if has valid value or given alternative value
		//		if has error
		template <typename U>
		constexpr T valueOr(U&& alt) &&
		{ 
			if (isOk())
				return std::move(value());
			else
				return static_cast<T>(std::forward<U>(alt));
		}

		// returns error if this stores an error, crashes otherwise :(
		constexpr E& error() { return std::get<E>(m_value); }

		// returns error if this stores an error, crashes otherwise :(
		constexpr const E& error() const { return std::get<E>(m_value); }

	private:
		std::variant<T, E> m_value;
	};

	//------------------------------------------------------------------------------
	template <typename E>
	class Result<void, E> final : public BaseObjectLiteralType<>
	{
	public:
		constexpr Result() = delete;
		constexpr Result(Result&) = default;
		constexpr Result(Result&&) = default;
		constexpr Result& operator=(Result&) = default;
		constexpr Result& operator=(Result&&) = default;

		// Ctor taking ok value. In fact it doesn't take any value it's just 
		// forces user to explicitly set state of this type of Result 
		// instance.
		// @param okVal - this is empty anyway
		constexpr Result(Ok<void> okVal) {}

		// Ctor taking error value. Object created with this ctor will return
		// false from isOk and true from isErr.
		// @param errVal - value for error/invalid result
		constexpr Result(Err<E> errVal) : m_value(std::move(errVal).get()) {}

		// Ctor taking error value. Object created with this ctor will return
		// false from isOk and true from isErr. Using this ctor rather than
		// the one taing Err<T> will save you one move operation.
		// @param errVal - value for error/invalid result
		explicit constexpr Result(E errVal) : m_value(std::move(errVal)) {}

		// @returns true if result stores valid value, false if stores error
		//		value
		constexpr bool isOk() const { return m_value.has_value() == false; }

		// @returns false if result stores valid value, true if stores error
		//		value
		constexpr bool isErr() const { return !isOk(); }

		// returns error if this stores an error, crashes otherwise :(
		constexpr E& error() { return m_value.value(); }

		// returns error if this stores an error, crashes otherwise :(
		constexpr const E& error() const { return m_value.value(); }

	private:
		std::optional<E> m_value;
	};

	/*namespace result
	{
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
	}*/
}
