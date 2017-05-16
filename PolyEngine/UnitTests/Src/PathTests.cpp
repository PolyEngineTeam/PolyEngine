#include <catch.hpp>

#include <Path.hpp>

using namespace Poly;

TEST_CASE("Path building", "[PATH]")
{
	Path p = Path::Init()->RootAt("C:").Dir("Dir1").Dir("Dir2").File("File.ext");
	REQUIRE(p.Get() == "C:\\Dir1\\Dir2\\File.ext");
	REQUIRE(p.IsAbsolute());
	REQUIRE(!p.IsDir());

	Path p2 = Path::Init()->Dir("Dir1").Dir("Dir2");
	REQUIRE(p2.Get() == "Dir1\\Dir2\\");
	REQUIRE(!p2.IsAbsolute());
	REQUIRE(p2.IsDir());

}