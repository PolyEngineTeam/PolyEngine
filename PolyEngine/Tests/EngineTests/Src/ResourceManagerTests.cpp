#include <pe/Defines.hpp>
#include <catch.hpp>

#define _WINDLL
#define _GAME //fake being a game for the dllexport macros
#include <pe/engine/res/ResourceManager.hpp>

using namespace pe::engine;

class DummyResource : public res::ResourceBase
{
public:
	DummyResource(const ::pe::core::storage::String& /*path*/) {}
};

namespace pe::engine::res {
	TEST_DECLARE_RESOURCE(DummyResource, gDummyResourcesMap)
}
DEFINE_RESOURCE(DummyResource, gDummyResourcesMap)

TEST_CASE("ResourceManager loading/freeing", "[ResourceManager]")
{
	DummyResource* res1 = res::ResourceManager<DummyResource>::Load("a", config::eResourceSource::NONE);
	DummyResource* res2 = res::ResourceManager<DummyResource>::Load("a", config::eResourceSource::NONE);
	DummyResource* res3 = res::ResourceManager<DummyResource>::Load("b", config::eResourceSource::NONE);
	DummyResource* res4 = res::ResourceManager<DummyResource>::Load("c", config::eResourceSource::NONE);

	REQUIRE(res1 == res2);
	REQUIRE(res1 != res3);
	REQUIRE(res3 != res4);
	REQUIRE(res1->GetRefCount() == 2);
	REQUIRE(res2->GetRefCount() == 2);
	REQUIRE(res3->GetRefCount() == 1);
	REQUIRE(res4->GetRefCount() == 1);

	res::ResourceManager<DummyResource>::Release(res2);
	res2 = nullptr;
	REQUIRE(res1->GetRefCount() == 1);
	DummyResource* res5 = res::ResourceManager<DummyResource>::Load("a", config::eResourceSource::NONE);
	REQUIRE(res1->GetRefCount() == 2);
	REQUIRE(res1 == res5);
	res::ResourceManager<DummyResource>::Release(res5);
	res2 = nullptr;
	REQUIRE(res1->GetRefCount() == 1);

	res::ResourceManager<DummyResource>::Release(res1);
	res::ResourceManager<DummyResource>::Release(res3);
	res::ResourceManager<DummyResource>::Release(res4);
}
