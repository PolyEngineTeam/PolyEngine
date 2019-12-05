#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/utils/Result.hpp>

using namespace pe::core::utils;

namespace
{
	enum class eTestErrorType
	{
		ERROR_TYPE_1,
		ERROR_TYPE_2
	};

	Result<bool, eTestErrorType> func(int in)
	{
		if (in < 10)
			return in > 5;
		else
			return eTestErrorType::ERROR_TYPE_1;
	}
}

TEST_CASE("Result.isOk", "[Result]") 
{
	Result<int, eTestErrorType> result1 = 43;
	REQUIRE(result1.isOk() == true);

	Result<int, eTestErrorType> result2 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result2.isOk() == false);

	Result<void, eTestErrorType> result3;
	REQUIRE(result3.isOk() == true);

	Result<void, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result4.isOk() == false);
}

TEST_CASE("Result.isErr", "[Result]")
{
	Result<int, eTestErrorType> result1 = 43;
	REQUIRE(result1.isErr() == false);

	Result<int, eTestErrorType> result2 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result2.isErr() == true);

	Result<void, eTestErrorType> result3;
	REQUIRE(result3.isErr() == false);

	Result<void, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result4.isErr() == true);
}

TEST_CASE("Result.getValue", "[Result]")
{
	Result<int, eTestErrorType> result1 = 43;
	REQUIRE(result1.getValue() == 43);
}

TEST_CASE("Result.getError", "[Result]")
{
	Result<int, eTestErrorType> result2 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result2.getError() == eTestErrorType::ERROR_TYPE_1);

	Result<void, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result4.getError() == eTestErrorType::ERROR_TYPE_1);
}

TEST_CASE("Result.join", "[Result]")
{
	Result<Result<int, eTestErrorType>, eTestErrorType> result1 = 43;
	REQUIRE(join(result1).getValue() == 43);

	Result<Result<int, eTestErrorType>, eTestErrorType> result2 = Result<int, eTestErrorType>(eTestErrorType::ERROR_TYPE_1);
	REQUIRE(join(result2).getError() == eTestErrorType::ERROR_TYPE_1);
	
	Result<Result<void, eTestErrorType>, eTestErrorType> result3 = Result<void, eTestErrorType>();
	REQUIRE(join(result3).isOk() == true);
	
	Result<Result<void, eTestErrorType>, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(join(result4).isOk() == false);

	Result<Result<void, eTestErrorType>, eTestErrorType> result5 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(join(result4).getError() == eTestErrorType::ERROR_TYPE_1);
}



TEST_CASE("Result.bind", "[Result]")
{
	Result<int, eTestErrorType> result1 = 2;
	REQUIRE(bind(result1, func).getValue() == false);

	Result<int, eTestErrorType> result2 = 7;
	REQUIRE(bind(result2, func).getValue() == true);

	Result<int, eTestErrorType> result3 = 11;
	REQUIRE(bind(result3, func).getError() == eTestErrorType::ERROR_TYPE_1);

	Result<int, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_2;
	REQUIRE(bind(result4, func).getError() == eTestErrorType::ERROR_TYPE_2);
}