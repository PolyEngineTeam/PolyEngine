#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/memory/PoolAllocator.hpp>
#include <pe/core/memory/IterablePoolAllocator.hpp>

TEST_CASE("Pool allocator", "[Allocator]") {
	::pe::core::memory::PoolAllocator<size_t> allocator(10);
	REQUIRE(allocator.GetSize() == 0);
	
	size_t* a = allocator.Alloc();
	REQUIRE(a != nullptr);
	REQUIRE(allocator.GetSize() == 1);
	
	size_t* b = allocator.Alloc();
	REQUIRE(b != nullptr);
	REQUIRE((a + 1) == b);
	REQUIRE(allocator.GetSize() == 2);
	
	size_t* c = allocator.Alloc();
	REQUIRE(c != nullptr);
	REQUIRE((a + 2) == c);
	REQUIRE((b + 1) == c);
	REQUIRE(allocator.GetSize() == 3);
	
	allocator.Free(b);
	REQUIRE(allocator.GetSize() == 2);
	size_t* d = allocator.Alloc();
	REQUIRE(d == b);
	REQUIRE(allocator.GetSize() == 3);
}

TEST_CASE("Iterable pool allocator", "[Allocator]") {
	
	// test allocation
	::pe::core::memory::IterablePoolAllocator<size_t> allocator(10);
	REQUIRE(allocator.GetSize() == 0);

	size_t* a = allocator.Alloc();
	REQUIRE(a != nullptr);
	REQUIRE(allocator.GetSize() == 1);

	size_t* b = allocator.Alloc();
	REQUIRE(b != nullptr);
	REQUIRE(allocator.GetSize() == 2);

	size_t* c = allocator.Alloc();
	REQUIRE(c != nullptr);
	REQUIRE(allocator.GetSize() == 3);

	allocator.Free(b);
	REQUIRE(allocator.GetSize() == 2);
	size_t* b_2 = allocator.Alloc();
	REQUIRE(b_2 == b);
	REQUIRE(allocator.GetSize() == 3);


	// test iterators
	*a = 1;
	*b_2 = 2;
	*c = 3;

	size_t i = 0;
	for (size_t val : allocator)
		REQUIRE(val == ++i);

	::pe::core::memory::IterablePoolAllocator<size_t> empty_allocator(10);
	REQUIRE(empty_allocator.Begin() == empty_allocator.End());
}

TEST_CASE("Iterable pool allocator corner case test", "[Allocator]") {
	// test allocation
	::pe::core::memory::IterablePoolAllocator<size_t> allocator(3);

	size_t* a = allocator.Alloc();
	REQUIRE(a != nullptr);
	REQUIRE(allocator.GetSize() == 1);

	size_t* b = allocator.Alloc();
	REQUIRE(b != nullptr);
	REQUIRE(allocator.GetSize() == 2);

	allocator.Free(a);
	REQUIRE(allocator.GetSize() == 1);

	size_t* c = allocator.Alloc();
	REQUIRE(c != nullptr);
	REQUIRE(allocator.GetSize() == 2);

	size_t* d = allocator.Alloc();
	REQUIRE(d != nullptr);
	REQUIRE(allocator.GetSize() == 3);

	allocator.Free(d);
	REQUIRE(allocator.GetSize() == 2);

	size_t* e = allocator.Alloc();
	REQUIRE(e != nullptr);
	REQUIRE(allocator.GetSize() == 3);
}