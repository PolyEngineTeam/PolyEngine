#include <catch.hpp>

#include <Path.hpp>

using namespace Poly;

TEST_CASE("Path daisychain", "[PATH]")
{
	Path p = (new Path("C:"))->Dir("Dir1").Dir("Dir2").File("File.ext");
	REQUIRE(p.Get() == "C:\\Dir1\\Dir2\\File.ext");

}