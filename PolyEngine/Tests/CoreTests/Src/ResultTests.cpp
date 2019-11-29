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

TEST_CASE("Result.isOk", "[Result]") 
{

	//Result<int, eTestErrorType> okResult = 45;
	auto res = Result<void, eTestErrorType>();
}