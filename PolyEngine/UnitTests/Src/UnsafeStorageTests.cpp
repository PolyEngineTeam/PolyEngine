#include <catch.hpp>

#include <UnsafeStorage.hpp>

using namespace Poly;

TEST_CASE("Unsafe Storage", "[UnsafeStorage]") {
	static size_t gCurrentInstances = 0;
	struct Counting {
		Counting(char value) : value(value) { gCurrentInstances += 1; }
		Counting(const Counting& other) : Counting(other.value) {}
		~Counting() { gCurrentInstances -= 1; }
		char value;
	};

	UnsafeStorage<Counting, 8> storage;
	size_t len = 0;

	REQUIRE(gCurrentInstances == 0);

	::new(&storage[0]) Counting(0);
	::new(&storage[1]) Counting(1);
	::new(&storage[2]) Counting(2);
	::new(&storage[3]) Counting(3);
	len  =  4;

	REQUIRE(gCurrentInstances == len);
	REQUIRE(storage[0].value == 0);
	REQUIRE(storage[1].value == 1);
	REQUIRE(storage[2].value == 2);
	REQUIRE(storage[3].value == 3);

	{
		size_t i = 0;
		for (auto it = storage.begin(); it != storage.begin() + len; ++it, ++i) {
			REQUIRE(it->value == i);
		}
	}

	storage.destruct(len);
	REQUIRE(gCurrentInstances == 0);
}
