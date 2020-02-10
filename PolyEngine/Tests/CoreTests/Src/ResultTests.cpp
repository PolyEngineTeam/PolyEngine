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
		constexpr CounterClass(const CounterClass& other)
		{
			m_ctor = other.m_ctor;
			m_copyCtor = other.m_copyCtor;
			m_moveCtor = other.m_moveCtor;
			m_copyOp = other.m_copyOp;
			m_moveOp = other.m_moveOp;

			++m_copyCtor;
		}
		constexpr CounterClass(const CounterClass&& other)
		{
			m_ctor = other.m_ctor;
			m_copyCtor = other.m_copyCtor;
			m_moveCtor = other.m_moveCtor;
			m_copyOp = other.m_copyOp;
			m_moveOp = other.m_moveOp;

			++m_moveCtor;
		}
		constexpr CounterClass& operator=(const CounterClass& other)
		{
			m_ctor = other.m_ctor;
			m_copyCtor = other.m_copyCtor;
			m_moveCtor = other.m_moveCtor;
			m_copyOp = other.m_copyOp;
			m_moveOp = other.m_moveOp;

			++m_copyOp;

			return *this;
		}
		constexpr CounterClass& operator=(const CounterClass&& other)
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

	constexpr Result res5 = Ok(CounterClass());
	constexpr bool isOk5 = res5.isOk();
	REQUIRE(isOk5 == true);

	constexpr Result<CounterClass> res6 = Err();
	constexpr bool isOk6 = res6.isOk();
	REQUIRE(isOk6 == false);

	constexpr Result res7 = Ok();
	constexpr bool isOk7 = res7.isOk();
	REQUIRE(isOk7 == true);

	constexpr Result<void> res8 = Err();
	constexpr bool isOk8 = res8.isOk();
	REQUIRE(isOk8 == false);
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

	constexpr Result res5 = Ok(CounterClass());
	constexpr bool isErr5 = res5.isErr();
	REQUIRE(isErr5 == false);

	constexpr Result<CounterClass> res6 = Err();
	constexpr bool isErr6 = res6.isErr();
	REQUIRE(isErr6 == true);

	constexpr Result res7 = Ok();
	constexpr bool isErr7 = res7.isErr();
	REQUIRE(isErr7 == false);

	constexpr Result<void> res8 = Err();
	constexpr bool isErr8 = res8.isErr();
	REQUIRE(isErr8 == true);
}

TEST_CASE("Result.value", "[Result]")
{
	constexpr Result res1 = Ok(CounterClass());
	constexpr const CounterClass& val1 = res1.value();
	REQUIRE(val1.m_ctor == 1);
	REQUIRE(val1.m_copyCtor == 0);
	REQUIRE(val1.m_moveCtor == 2);
	REQUIRE(val1.m_copyOp == 0);
	REQUIRE(val1.m_moveOp == 0);

	constexpr Result res2 = Result<CounterClass>(CounterClass());
	constexpr const CounterClass& val2 = res2.value();
	REQUIRE(val2.m_ctor == 1);
	REQUIRE(val2.m_copyCtor == 0);
	REQUIRE(val2.m_moveCtor == 1);
	REQUIRE(val2.m_copyOp == 0);
	REQUIRE(val2.m_moveOp == 0);
}

TEST_CASE("Result.valueOr", "[Result]")
{	
	constexpr Result res1 = Ok(1);
	constexpr int val1 = res1.valueOr(10);
	REQUIRE(val1 == 1);

	constexpr Result<int> res2 = Err();
	constexpr int val2 = res2.valueOr(20);
	REQUIRE(val2 == 20);

	constexpr Result res3 = Ok(3);
	constexpr int val3 = std::move(res3).valueOr(30);
	REQUIRE(val3 == 3);

	constexpr Result<int> res4 = Err();
	constexpr int val4 = res4.valueOr(40);
	REQUIRE(val4 == 40);
	
	constexpr Result res5 = Ok(CounterClass());
	constexpr const CounterClass& val5 = res5.valueOr(CounterClass());
	REQUIRE(val5.m_ctor == 1);
	REQUIRE(val5.m_copyCtor == 1);
	REQUIRE(val5.m_moveCtor == 2);
	REQUIRE(val5.m_copyOp == 0);
	REQUIRE(val5.m_moveOp == 0);

	constexpr Result<CounterClass> res6 = Err();
	constexpr const CounterClass& val6 = res6.valueOr(CounterClass());
	REQUIRE(val6.m_ctor == 1);
	REQUIRE(val6.m_copyCtor == 0);
	REQUIRE(val6.m_moveCtor == 1);
	REQUIRE(val6.m_copyOp == 0);
	REQUIRE(val6.m_moveOp == 0);
	
	constexpr Result<CounterClass, eTestErrorType> res7 = Result<CounterClass, eTestErrorType>(CounterClass());
	constexpr const CounterClass& val7 = res7.valueOr(CounterClass());
	REQUIRE(val7.m_ctor == 1);
	REQUIRE(val7.m_copyCtor == 1);
	REQUIRE(val7.m_moveCtor == 1);
	REQUIRE(val7.m_copyOp == 0);
	REQUIRE(val7.m_moveOp == 0);

	Result res8 = Ok(CounterClass());
	const CounterClass& val8 = std::move(res8).valueOr(CounterClass());
	REQUIRE(val8.m_ctor == 1);
	REQUIRE(val8.m_copyCtor == 0);
	REQUIRE(val8.m_moveCtor == 3);
	REQUIRE(val8.m_copyOp == 0);
	REQUIRE(val8.m_moveOp == 0);

	Result<CounterClass> res9 = Err();
	const CounterClass& val9 = std::move(res9).valueOr(CounterClass());
	REQUIRE(val9.m_ctor == 1);
	REQUIRE(val9.m_copyCtor == 0);
	REQUIRE(val9.m_moveCtor == 1);
	REQUIRE(val9.m_copyOp == 0);
	REQUIRE(val9.m_moveOp == 0);

	Result<CounterClass, eTestErrorType> res10 = Result<CounterClass, eTestErrorType>(CounterClass());
	const CounterClass& val10 = std::move(res10).valueOr(CounterClass());
	REQUIRE(val10.m_ctor == 1);
	REQUIRE(val10.m_copyCtor == 0);
	REQUIRE(val10.m_moveCtor == 2);
	REQUIRE(val10.m_copyOp == 0);
	REQUIRE(val10.m_moveOp == 0);
}

TEST_CASE("Result.error", "[Result]")
{
	constexpr Result<CounterClass, eTestErrorType> res1 = Err(eTestErrorType::ERROR_TYPE_1);
	constexpr eTestErrorType err1 = res1.error();
	REQUIRE(err1 == eTestErrorType::ERROR_TYPE_1);

	constexpr Result<void, eTestErrorType> res2 = Err(eTestErrorType::ERROR_TYPE_1);
	constexpr eTestErrorType err2 = res2.error();
	REQUIRE(err2 == eTestErrorType::ERROR_TYPE_1);

	constexpr Result<CounterClass> res3 = Err();
	constexpr const char* err3 = res3.error();
	REQUIRE(strcmp(err3, Err<>::DEFAULT_ERROR_MESSAGE) == 0);

	constexpr Result<void> res4 = Err();
	constexpr const char* err4 = res4.error();
	REQUIRE(strcmp(err4, Err<>::DEFAULT_ERROR_MESSAGE) == 0);

	constexpr const char* CUSTOM_ERROR_MESSAGE = "Custom error message";
	constexpr Result<CounterClass> res5 = Err(CUSTOM_ERROR_MESSAGE);
	constexpr const char* err5 = res5.error();
	REQUIRE(strcmp(err5, CUSTOM_ERROR_MESSAGE) == 0);

	constexpr Result<void> res6 = Err(CUSTOM_ERROR_MESSAGE);
	constexpr const char* err6 = res6.error();
	REQUIRE(strcmp(err6, CUSTOM_ERROR_MESSAGE) == 0);
}

TEST_CASE("Result.bind", "[Result]")
{
	std::function okFunc = [] (int i) -> Result<CounterClass, eTestErrorType> { return Ok(CounterClass()); };

	Result<CounterClass, eTestErrorType> res = bind(okFunc, Result<int, eTestErrorType>(5));

	//REQUIRE(val5.m_ctor == 1);
	//REQUIRE(val5.m_copyCtor == 1);
	//REQUIRE(val5.m_moveCtor == 2);
	//REQUIRE(val5.m_copyOp == 0);
	//REQUIRE(val5.m_moveOp == 0);
}
