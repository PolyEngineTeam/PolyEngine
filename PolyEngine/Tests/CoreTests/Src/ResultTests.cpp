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
	REQUIRE(result1.IsOk() == true);

	Result<int, eTestErrorType> result2 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result2.IsOk() == false);

	Result<void, eTestErrorType> result3;
	REQUIRE(result3.IsOk() == true);

	Result<void, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result4.IsOk() == false);
}

TEST_CASE("Result.IsErr", "[Result]")
{
	Result<int, eTestErrorType> result1 = 43;
	REQUIRE(result1.IsErr() == false);

	Result<int, eTestErrorType> result2 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result2.IsErr() == true);

	Result<void, eTestErrorType> result3;
	REQUIRE(result3.IsErr() == false);

	Result<void, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result4.IsErr() == true);
}

TEST_CASE("Result.GetValue", "[Result]")
{
	Result<int, eTestErrorType> result1 = 43;
	REQUIRE(result1.GetValue() == 43);
}

TEST_CASE("Result.GetError", "[Result]")
{
	Result<int, eTestErrorType> result2 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result2.GetError() == eTestErrorType::ERROR_TYPE_1);

	Result<void, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result4.GetError() == eTestErrorType::ERROR_TYPE_1);
}