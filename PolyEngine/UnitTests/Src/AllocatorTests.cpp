#include <catch.hpp>

#include <Allocator.hpp>

using namespace Poly;

TEST_CASE("Pool allocator", "[Allocator]") {
	PoolAllocator<int> allocator(10);
	REQUIRE(allocator.GetSize() == 0);
	
	int* a = allocator.Alloc();
	REQUIRE(a != nullptr);
	REQUIRE(allocator.GetSize() == 1);
	
	int* b = allocator.Alloc();
	REQUIRE(b != nullptr);
	REQUIRE((a + 1) == b);
	REQUIRE(allocator.GetSize() == 2);
	
	int* c = allocator.Alloc();
	REQUIRE(c != nullptr);
	REQUIRE((a + 2) == c);
	REQUIRE((b + 1) == c);
	REQUIRE(allocator.GetSize() == 3);
	
	allocator.Free(b);
	REQUIRE(allocator.GetSize() == 2);
	int* d = allocator.Alloc();
	REQUIRE(d == b);
	REQUIRE(allocator.GetSize() == 3);
}