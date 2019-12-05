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
}

TEST_CASE("Result.IsOk", "[Result]") 
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

TEST_CASE("Result.IsErr", "[Result]")
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

TEST_CASE("Result.GetValue", "[Result]")
{
	Result<int, eTestErrorType> result1 = 43;
	REQUIRE(result1.getValue() == 43);
}

TEST_CASE("Result.GetError", "[Result]")
{
	Result<int, eTestErrorType> result2 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result2.getError() == eTestErrorType::ERROR_TYPE_1);

	Result<void, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result4.getError() == eTestErrorType::ERROR_TYPE_1);
}