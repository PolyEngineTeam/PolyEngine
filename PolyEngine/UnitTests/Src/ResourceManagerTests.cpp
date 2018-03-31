#include <Defines.hpp>
#include <catch.hpp>

#define _WINDLL
#define _GAME //fake being a game for the dllexport macros
#include <Resources/ResourceManager.hpp>

using namespace Poly;

class DummyResource : public Poly::ResourceBase
{
public:
	DummyResource(const String& /*path*/) {}
};

namespace Poly {
	TEST_DECLARE_RESOURCE(DummyResource, gDummyResourcesMap)
}
DEFINE_RESOURCE(DummyResource, gDummyResourcesMap)

TEST_CASE("ResourceManager loading/freeing", "[ResourceManager]")
{
	DummyResource* res1 = ResourceManager<DummyResource>::Load("a", eResourceSource::NONE);
	DummyResource* res2 = ResourceManager<DummyResource>::Load("a", eResourceSource::NONE);
	DummyResource* res3 = ResourceManager<DummyResource>::Load("b", eResourceSource::NONE);
	DummyResource* res4 = ResourceManager<DummyResource>::Load("c", eResourceSource::NONE);

	REQUIRE(res1 == res2);
	REQUIRE(res1 != res3);
	REQUIRE(res3 != res4);
	REQUIRE(res1->GetRefCount() == 2);
	REQUIRE(res2->GetRefCount() == 2);
	REQUIRE(res3->GetRefCount() == 1);
	REQUIRE(res4->GetRefCount() == 1);

	ResourceManager<DummyResource>::Release(res2);
	res2 = nullptr;
	REQUIRE(res1->GetRefCount() == 1);
	DummyResource* res5 = ResourceManager<DummyResource>::Load("a", eResourceSource::NONE);
	REQUIRE(res1->GetRefCount() == 2);
	REQUIRE(res1 == res5);
	ResourceManager<DummyResource>::Release(res5);
	res2 = nullptr;
	REQUIRE(res1->GetRefCount() == 1);

	ResourceManager<DummyResource>::Release(res1);
	ResourceManager<DummyResource>::Release(res3);
	ResourceManager<DummyResource>::Release(res4);
}
