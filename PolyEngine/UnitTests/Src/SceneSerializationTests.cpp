#include <Defines.hpp>
#include <catch.hpp>

#include <RTTI/RTTI.hpp>
#include <ECS/World.hpp>
#include <cstdio>

using namespace Poly;

TEST_CASE("Scene serialization tests", "[Scene]")
{
	Scene s;
	s.SerializeToFile("Scene.test");
}