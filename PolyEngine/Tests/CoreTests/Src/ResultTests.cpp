#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/utils/Result.hpp>

using namespace pe::core::utils;

namespace
{
	class CounterClass
	{
	public:
		constexpr CounterClass() { ++m_ctor; }
		constexpr CounterClass(CounterClass& other)
		{
			m_ctor = other.m_ctor;
			m_copyCtor = other.m_copyCtor;
			m_moveCtor = other.m_moveCtor;
			m_copyOp = other.m_copyOp;
			m_moveOp = other.m_moveOp;

			++m_copyCtor;
		}
		constexpr CounterClass(CounterClass&& other)
		{
			m_ctor = other.m_ctor;
			m_copyCtor = other.m_copyCtor;
			m_moveCtor = other.m_moveCtor;
			m_copyOp = other.m_copyOp;
			m_moveOp = other.m_moveOp;

			++m_moveCtor;
		}
		constexpr CounterClass& operator=(CounterClass& other)
		{
			m_ctor = other.m_ctor;
			m_copyCtor = other.m_copyCtor;
			m_moveCtor = other.m_moveCtor;
			m_copyOp = other.m_copyOp;
			m_moveOp = other.m_moveOp;

			++m_copyOp;

			return *this;
		}
		constexpr CounterClass& operator=(CounterClass&& other)
		{
			m_ctor = other.m_ctor;
			m_copyCtor = other.m_copyCtor;
			m_moveCtor = other.m_moveCtor;
			m_copyOp = other.m_copyOp;
			m_moveOp = other.m_moveOp;

			++m_moveOp;

			return *this;
		}

		int m_ctor = 0;
		int m_copyCtor = 0;
		int m_moveCtor = 0;
		int m_copyOp = 0;
		int m_moveOp = 0;
	};

	enum class eTestErrorType
	{
		ERROR_TYPE_1,
		ERROR_TYPE_2
	};

	int simpleFunc()
	{
		return 0;
	}

	Result<bool, eTestErrorType> func(int in)
	{
		if (in < 10)
			return Ok(in > 5);
		else
			return Err(eTestErrorType::ERROR_TYPE_1);
	}

	int func5(int a, float b, Result<bool, eTestErrorType> c, float d, std::string e)
	{
		return 0;
	}
}

TEST_CASE("Result.isOk", "[Result]")
{
	constexpr Result<CounterClass, eTestErrorType> res1 = Ok(CounterClass());
	constexpr bool isOk1 = res1.isOk();
	REQUIRE(isOk1 == true);

	constexpr Result<CounterClass, eTestErrorType> res2 = Err(eTestErrorType::ERROR_TYPE_1);
	constexpr bool isOk2 = res2.isOk();
	REQUIRE(isOk2 == false);

	constexpr Result<void, eTestErrorType> res3 = Ok();
	constexpr bool isOk3 = res3.isOk();
	REQUIRE(isOk3 == true);

	constexpr Result<void, eTestErrorType> res4 = Err(eTestErrorType::ERROR_TYPE_1);
	constexpr bool isOk4 = res4.isOk();
	REQUIRE(isOk4 == false);
}

TEST_CASE("Result.isErr", "[Result]")
{
	constexpr Result<CounterClass, eTestErrorType> res1 = Ok(CounterClass());
	constexpr bool isErr1 = res1.isErr();
	REQUIRE(isErr1 == false);

	constexpr Result<CounterClass, eTestErrorType> res2 = Err(eTestErrorType::ERROR_TYPE_1);
	constexpr bool isErr2 = res2.isErr();
	REQUIRE(isErr2 == true);

	constexpr Result<void, eTestErrorType> res3 = Ok();
	constexpr bool isErr3 = res3.isErr();
	REQUIRE(isErr3 == false);

	constexpr Result<void, eTestErrorType> res4 = Err(eTestErrorType::ERROR_TYPE_1);
	constexpr bool isErr4 = res4.isErr();
	REQUIRE(isErr4 == true);
}

TEST_CASE("Result.value", "[Result]")
{
	constexpr Result<CounterClass, eTestErrorType> res1 = Ok(CounterClass());
	constexpr const CounterClass& val1 = res1.value();
	REQUIRE(val1.m_ctor == 1);
	REQUIRE(val1.m_copyCtor == 0);
	REQUIRE(val1.m_moveCtor == 2);
	REQUIRE(val1.m_copyOp == 0);
	REQUIRE(val1.m_moveOp == 0);

	constexpr Result<CounterClass, eTestErrorType> res2 = Result<CounterClass, eTestErrorType>(CounterClass());
	constexpr const CounterClass & val2 = res2.value();
	REQUIRE(val2.m_ctor == 1);
	REQUIRE(val2.m_copyCtor == 0);
	REQUIRE(val2.m_moveCtor == 1);
	REQUIRE(val2.m_copyOp == 0);
	REQUIRE(val2.m_moveOp == 0);
}

TEST_CASE("Result.valueOr", "[Result]")
{
	constexpr Result<CounterClass, eTestErrorType> res1 = Ok(CounterClass());
	constexpr const CounterClass& val1 = res1.value();
	REQUIRE(val1.m_ctor == 1);
	REQUIRE(val1.m_copyCtor == 0);
	REQUIRE(val1.m_moveCtor == 2);
	REQUIRE(val1.m_copyOp == 0);
	REQUIRE(val1.m_moveOp == 0);

	constexpr Result<CounterClass, eTestErrorType> res2 = Result<CounterClass, eTestErrorType>(CounterClass());
	constexpr const CounterClass & val2 = res2.value();
	REQUIRE(val2.m_ctor == 1);
	REQUIRE(val2.m_copyCtor == 0);
	REQUIRE(val2.m_moveCtor == 1);
	REQUIRE(val2.m_copyOp == 0);
	REQUIRE(val2.m_moveOp == 0);
}

//TEST_CASE("Result.getValue", "[Result]")
//{
//	Result<int, eTestErrorType> result1 = Ok(43);
//	REQUIRE(result1.getValue() == 43);
//}
//
//TEST_CASE("Result.getError", "[Result]")
//{
//	Result<int, eTestErrorType> result2 = Err(eTestErrorType::ERROR_TYPE_1);
//	REQUIRE(result2.getError() == eTestErrorType::ERROR_TYPE_1);
//
//	Result<void, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
//	REQUIRE(result4.getError() == eTestErrorType::ERROR_TYPE_1);
//}
//
//TEST_CASE("Result.join", "[Result]")
//{
//	Result<Result<int, eTestErrorType>, eTestErrorType> result1 = Result<int, eTestErrorType>(43);
//	REQUIRE(result::join(result1).getValue() == 43);
//
//	Result<Result<int, eTestErrorType>, eTestErrorType> result2 = Result<int, eTestErrorType>(eTestErrorType::ERROR_TYPE_1);
//	REQUIRE(result::join(result2).getError() == eTestErrorType::ERROR_TYPE_1);
//	
//	Result<Result<void, eTestErrorType>, eTestErrorType> result3 = Result<void, eTestErrorType>();
//	REQUIRE(result::join(result3).isOk() == true);
//	
//	Result<Result<void, eTestErrorType>, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
//	REQUIRE(result::join(result4).isOk() == false);
//
//	Result<Result<void, eTestErrorType>, eTestErrorType> result5 = eTestErrorType::ERROR_TYPE_1;
//	REQUIRE(result::join(result4).getError() == eTestErrorType::ERROR_TYPE_1);
//}
//
//TEST_CASE("Result.bind", "[Result]")
//{
//	Result<int, eTestErrorType> result1 = 2;
//	REQUIRE(result::bind(result1, func).getValue() == false);
//
//	Result<int, eTestErrorType> result2 = 7;
//	REQUIRE(result::bind(result2, func).getValue() == true);
//
//	Result<int, eTestErrorType> result3 = 11;
//	REQUIRE(result::bind(result3, func).getError() == eTestErrorType::ERROR_TYPE_1);
//
//	Result<int, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_2;
//	REQUIRE(result::bind(result4, func).getError() == eTestErrorType::ERROR_TYPE_2);
//}
//
//TEST_CASE("Result.first", "[Result]")
//{
//	{
//		Result<int, eTestErrorType> result1 = 2;
//		Result<bool, eTestErrorType> result2 = true;
//		REQUIRE(result1.first(result2).getValue() == true);
//
//		Result<int, eTestErrorType> result3 = eTestErrorType::ERROR_TYPE_1;
//		Result<bool, eTestErrorType> result4 = true;
//		REQUIRE(result3.first(result4).getError() == eTestErrorType::ERROR_TYPE_1);
//
//		Result<bool, eTestErrorType> result5 = true;
//		Result<int, eTestErrorType> result6 = eTestErrorType::ERROR_TYPE_2;
//		REQUIRE(result5.first(result6).getError() == eTestErrorType::ERROR_TYPE_2);
//
//		Result<bool, eTestErrorType> result7 = eTestErrorType::ERROR_TYPE_1;
//		Result<int, eTestErrorType> result8 = eTestErrorType::ERROR_TYPE_2;
//		REQUIRE(result7.first(result8).getError() == eTestErrorType::ERROR_TYPE_1);
//	}
//	{
//		Result<void, eTestErrorType> result1;
//		Result<bool, eTestErrorType> result2 = true;
//		REQUIRE(result1.first(result2).getValue() == true);
//
//		Result<void, eTestErrorType> result3 = eTestErrorType::ERROR_TYPE_1;
//		Result<bool, eTestErrorType> result4 = true;
//		REQUIRE(result3.first(result4).getError() == eTestErrorType::ERROR_TYPE_1);
//
//		Result<void, eTestErrorType> result5;
//		Result<int, eTestErrorType> result6 = eTestErrorType::ERROR_TYPE_2;
//		REQUIRE(result5.first(result6).getError() == eTestErrorType::ERROR_TYPE_2);
//
//		Result<void, eTestErrorType> result7 = eTestErrorType::ERROR_TYPE_1;
//		Result<int, eTestErrorType> result8 = eTestErrorType::ERROR_TYPE_2;
//		REQUIRE(result7.first(result8).getError() == eTestErrorType::ERROR_TYPE_1);
//	}
//	{
//		Result<int, eTestErrorType> result1 = 2;
//		Result<void, eTestErrorType> result2;
//		REQUIRE(result1.first(result2).isOk() == true);
//
//		Result<int, eTestErrorType> result3 = eTestErrorType::ERROR_TYPE_1;
//		Result<void, eTestErrorType> result4;
//		REQUIRE(result3.first(result4).getError() == eTestErrorType::ERROR_TYPE_1);
//
//		Result<bool, eTestErrorType> result5 = true;
//		Result<void, eTestErrorType> result6 = eTestErrorType::ERROR_TYPE_2;
//		REQUIRE(result5.first(result6).getError() == eTestErrorType::ERROR_TYPE_2);
//
//		Result<bool, eTestErrorType> result7 = eTestErrorType::ERROR_TYPE_1;
//		Result<void, eTestErrorType> result8 = eTestErrorType::ERROR_TYPE_2;
//		REQUIRE(result7.first(result8).getError() == eTestErrorType::ERROR_TYPE_1);
//	}
//}
//
//TEST_CASE("Result.last", "[Result]")
//{
//	{
//		Result<int, eTestErrorType> result1 = 2;
//		Result<int, eTestErrorType> result2 = true;
//		REQUIRE(result1.last(result2).getValue() == 2);
//
//		Result<int, eTestErrorType> result3 = eTestErrorType::ERROR_TYPE_1;
//		Result<int, eTestErrorType> result4 = 3;
//		REQUIRE(result3.last(result4).getValue() == 3);
//	
//		Result<int, eTestErrorType> result5 = 4;
//		Result<int, eTestErrorType> result6 = eTestErrorType::ERROR_TYPE_2;
//		REQUIRE(result5.last(result6).getValue() == 4);
//	
//		Result<int, eTestErrorType> result7 = eTestErrorType::ERROR_TYPE_1;
//		Result<int, eTestErrorType> result8 = eTestErrorType::ERROR_TYPE_2;
//		REQUIRE(result7.last(result8).getError() == eTestErrorType::ERROR_TYPE_2);
//	}
//	{
//		Result<void, eTestErrorType> result1;
//		Result<void, eTestErrorType> result2;
//		REQUIRE(result1.last(result2).isOk() == true);
//	
//		Result<void, eTestErrorType> result3 = eTestErrorType::ERROR_TYPE_1;
//		Result<void, eTestErrorType> result4;
//		REQUIRE(result3.last(result4).isOk() == true);
//	
//		Result<void, eTestErrorType> result5;
//		Result<void, eTestErrorType> result6 = eTestErrorType::ERROR_TYPE_2;
//		REQUIRE(result5.last(result6).isOk() == true);
//		
//		Result<void, eTestErrorType> result7 = eTestErrorType::ERROR_TYPE_1;
//		Result<void, eTestErrorType> result8 = eTestErrorType::ERROR_TYPE_2;
//		REQUIRE(result7.last(result8).getError() == eTestErrorType::ERROR_TYPE_2);
//	}
//}
//
//TEST_CASE("Result.lift", "[Result]")
//{
//	//LiftedFuncFactory::
//	auto method = result::lift<eTestErrorType>(&simpleFunc);
//	auto method2 = result::lift<eTestErrorType>(&func);
//	auto method3 = result::lift<eTestErrorType>(&func5);
//	method();
//	method2(3);
//	method3(1, 2.f, Result<bool, eTestErrorType>(false), 4.f, std::string("a"));
//}