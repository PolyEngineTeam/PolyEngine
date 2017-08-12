#include <catch.hpp>

#include <BTreeMap.hpp>
#include <Dynarray.hpp>
#include <random>


using namespace Poly;

TEST_CASE("BTreeMap sorted insertion/lookup", "[BTreeMap]") {
	constexpr size_t size = 1024;

	BTreeMap<int, int> map;
	for (int n = 0; n < int(size); ++n) {
		auto previous = map.Insert(n, -n);
		REQUIRE_FALSE(previous);
	}
	REQUIRE(map.GetSize() == size);

	AND_THEN("Lookup") {
		for (int n = 0; n < int(size); ++n) {
			auto got = map.Get(n);
			REQUIRE(got);
			REQUIRE(got.Value() == -n);
		}
	}

	AND_THEN("Replacement insertion") {

		for (int n = 0; n < int(size); ++n) {
			auto previous = map.Insert(n, n);
			REQUIRE(previous);
			REQUIRE(previous.Value() == -n);
		}
		REQUIRE(map.GetSize() == size);

		AND_THEN("Replacement lookup") {
			for (int n = 0; n < int(size); ++n) {
				auto got = map.Get(n);
				REQUIRE(got);
				REQUIRE(got.Value() == n);
			}
		}
	}

	AND_THEN("Iteration/ordering check") {
		size_t i = 0;
		for (auto kv : map) {
			REQUIRE(kv.key == -kv.value);
			REQUIRE(kv.key == i++);
		}
	}
}

TEST_CASE("BTreeMap random insertion/lookup", "[BTreeMap]") {
	constexpr size_t size = 1024;

	Dynarray<int> shuffled_input;
	shuffled_input.Reserve(size);
	for (int n = 0; n < int(size); ++n) {
		shuffled_input.PushBack(n);
	}

	auto mtSeed = std::rand(); //note(vuko): std::rand() is seeded by Catch; in case of test failure you can use `--rng-seed` to reproduce the results
	std::mt19937 rng(mtSeed); //note(vuko): a single int is actually by far too little data to properly seed the Twister, but we don't care about proper randomness here
	std::shuffle(shuffled_input.Begin(), shuffled_input.End(), rng);

	BTreeMap<int, int> map;
	for (int key : shuffled_input) {
		int value = -key;
		auto previous = map.Insert(key, value);
		REQUIRE_FALSE(previous);
	}
	REQUIRE(map.GetSize() == size);

	AND_THEN("Lookup") {
		for (int key = 0; key < int(size); ++key) {
			int value = -key;
			auto got = map.Get(key);
			REQUIRE(got);
			REQUIRE(got.Value() == value);
		}
	}

	AND_THEN("Replacement insertion") {

		for (int n = 0; n < int(size); ++n) {
			auto previous = map.Insert(n, n);
			REQUIRE(previous);
			REQUIRE(previous.Value() == -n);
		}
		REQUIRE(map.GetSize() == size);

		AND_THEN("Replacement lookup") {
			for (int n = 0; n < int(size); ++n) {
				auto got = map.Get(n);
				REQUIRE(got);
				REQUIRE(got.Value() == n);
			}
		}
	}

	AND_THEN("Iteration/ordering check") {
		size_t i = 0;
		for (auto kv : map) {
			REQUIRE(kv.key == -kv.value);
			REQUIRE(kv.key == i++);
		}
	}
}

TEST_CASE("BTreeMap clearing", "[BTreeMap]") {
	BTreeMap<int, int> map;

	SECTION("Size: B") {
		constexpr int size = decltype(map)::B;
		for (int i = 0; i < size; ++i) {
			map.Insert(i, i);
		}
		REQUIRE(map.GetSize() == size);

		map.Clear();
		REQUIRE(map.GetSize() == 0);
		REQUIRE_FALSE(map.Get(0));
	}

	SECTION("Size: 2*B") {
		constexpr int size = 2 * decltype(map)::B;
		for (int i = 0; i < size; ++i) {
			map.Insert(i, i);
		}
		REQUIRE(map.GetSize() == size);

		map.Clear();
		REQUIRE(map.GetSize() == 0);
		REQUIRE_FALSE(map.Get(0));
	}

	constexpr size_t size = 1024;
	SECTION("Size: 1024") {
		for (int i = 0; i < int(size); ++i) {
			map.Insert(i, i);
		}
		REQUIRE(map.GetSize() == size);

		map.Clear();
		REQUIRE(map.GetSize() == 0);
		REQUIRE_FALSE(map.Get(0));
	}
}

TEST_CASE("BTree random removals", "[BTreeMap]") {
	constexpr size_t size = 1024;

	Dynarray<int> shuffled_input;
	shuffled_input.Reserve(size);
	for (int n = 0; n < int(size); ++n) {
		shuffled_input.PushBack(n);
	}

	auto mtSeed = std::rand(); //note(vuko): std::rand() is seeded by Catch; in case of test failure you can use `--rng-seed` to reproduce the results
	std::mt19937 rng(mtSeed); //note(vuko): a single int is actually by far too little data to properly seed the Twister, but we don't care about proper randomness here
	std::shuffle(shuffled_input.Begin(), shuffled_input.End(), rng);

	BTreeMap<int, int> map;
	for (int key : shuffled_input) {
		int value = -key;
		map.Insert(key, value);
	}
	REQUIRE(map.GetSize() == size);
	REQUIRE(map.Get(0));

	Dynarray<int> to_remove;
	Dynarray<int> to_preserve;
	to_remove.Resize(size/2);
	to_preserve.Resize(size - size/2);
	std::copy(shuffled_input.Begin(), shuffled_input.Begin() + size/2, to_remove.Begin());
	std::copy(shuffled_input.Begin() + size/2, shuffled_input.End(), to_preserve.Begin());
	std::shuffle(to_remove  .Begin(), to_remove  .End(), rng);
	std::shuffle(to_preserve.Begin(), to_preserve.End(), rng);

	for (int key : to_remove) {
		int value = -key;
		auto removed = map.Remove(key);
		REQUIRE(removed);
		REQUIRE(removed.Value() == value);
		REQUIRE_FALSE(map.GetSize() == size);
	}
	REQUIRE(map.GetSize() == size - size/2);

	for (int key : to_remove) {
		REQUIRE_FALSE(map.Get(key));
	}

	for (int key : to_preserve) {
		int value = -key;
		auto got = map.Get(key);
		REQUIRE(got);
		REQUIRE(got.Value() == value);
	}

	std::sort(to_preserve.Begin(), to_preserve.End());
	{
		size_t i = 0;
		for (auto kv : map) {
			REQUIRE(kv.key == -kv.value);
			REQUIRE(kv.key == to_preserve[i++]);
		}
	}
}

TEST_CASE("BTreeMap properly running destructors", "[BTreeMap]") {
	//note(vuko): Valgrind + sanitizers (Mem/UB+Leak+Addr) detect no leaks, no uninitialized memory reads, no undefined behaviour.
	//            Any bugs (if there are any at all) must lie somewhere deeper...
	static size_t gCurrentInstances = 0; //counts living object at the given time
	static size_t gConstructorsRan  = 0;
	static size_t gDestructorsRan   = 0;
	struct Counting {
		Counting(size_t value) : value(value) { gCurrentInstances += 1; gConstructorsRan += 1; }
		Counting(const Counting&  other) : Counting(other.value) {}
		Counting(      Counting&& other) : Counting(std::move(other.value)) {}
		~Counting() { gCurrentInstances -= 1; gDestructorsRan += 1; }
		Counting& operator=(const Counting&  other) { this->value = other.value; return *this; }
		Counting& operator=(      Counting&& other) { this->value = other.value; return *this; }
		bool operator==(const Counting& other) const { return this->value == other.value; }
		bool operator<( const Counting& other) const { return this->value < other.value; }
		size_t value;
	};

	constexpr size_t size = 1024;
	BTreeMap<Counting, Counting> map;

	SECTION("Linear insertion") {
		for (size_t i = 0; i < size; ++i) {
			map.Insert(Counting(i), Counting(i));
		}
		REQUIRE(gCurrentInstances == map.GetSize() * 2);
	}

	SECTION("Non-linear insertion") {
		gCurrentInstances = 0;

		Dynarray<size_t> shuffled_input;
		shuffled_input.Reserve(size);
		for (size_t n = 0; n < size; ++n) {
			shuffled_input.PushBack(n);
		}

		auto mtSeed = std::rand(); //note(vuko): std::rand() is seeded by Catch; in case of test failure you can use `--rng-seed` to reproduce the results
		std::mt19937 rng(mtSeed); //note(vuko): a single int is actually by far too little data to properly seed the Twister, but we don't care about proper randomness here
		std::shuffle(shuffled_input.Begin(), shuffled_input.End(), rng);

		for (size_t v : shuffled_input) {
			map.Insert(Counting(v), Counting(v));
		}
		REQUIRE(gCurrentInstances == map.GetSize() * 2);

		AND_THEN("Clearing") {
			map.Clear();
			REQUIRE(gCurrentInstances == 0);
		}

		AND_THEN("Removals (half then rest)") {
			Dynarray<size_t> to_remove_first;
			Dynarray<size_t> to_remove_second;
			to_remove_first.Resize(size/2);
			to_remove_second.Resize(size - size/2);
			std::copy(shuffled_input.Begin(), shuffled_input.Begin() + size/2, to_remove_first.Begin());
			std::copy(shuffled_input.Begin() + size/2, shuffled_input.End(), to_remove_second.Begin());
			std::shuffle(to_remove_first .Begin(), to_remove_first .End(), rng);
			std::shuffle(to_remove_second.Begin(), to_remove_second.End(), rng);

			for (size_t v : to_remove_first) {
				map.Remove(Counting(v));
			}
			REQUIRE(gCurrentInstances == map.GetSize() * 2);

			for (size_t v : to_remove_second) {
				map.Remove(Counting(v));
			}
			REQUIRE(gCurrentInstances == 0);
		}

	}
}

TEST_CASE("BTreeMap avoiding copies", "[BTreeMap]") {
	//todo(vuko): test BTreeMap avoiding copies
}

TEST_CASE("BTreeMap bi-directional iteration", "[BTreeMap]") {
	BTreeMap<int, int> map;
	map.Insert(0, 0);
	map.Insert(1, 0);
	map.Insert(2, 0);
	map.Insert(3, 0);

	auto iter = map.Keys().begin();
	auto end  = map.Keys().end();

	SECTION("Change direction") {
		REQUIRE(*iter == 0);

		++iter; REQUIRE(*iter == 1);
		++iter; REQUIRE(*iter == 2);
		--iter; REQUIRE(*iter == 1);
		--iter; REQUIRE(*iter == 0);
	}

	SECTION("Interweave") {
		++iter; //1
		++iter; //2
		--iter; //1
		++iter; //2
		REQUIRE(*iter == 2);

		++iter; //3
		--iter; //2
		--iter; //1
		++iter; //2
		REQUIRE(*iter == 2);
	}

	SECTION("Negative bound") {
		--iter; REQUIRE(*iter == 0);
		--iter; REQUIRE(*iter == 0);
	}

	SECTION("Positive bound") {
		++iter; ++iter; ++iter; //1, 2, 3
		REQUIRE(*iter == 3);

		++iter; REQUIRE(iter == end);
		--iter; REQUIRE(*iter == 3);
	}

	SECTION("Both bounds") {
		--iter;
		REQUIRE(*iter == 0);

		--iter;
		REQUIRE(*iter == 0);

		++iter; //1
		REQUIRE(*iter == 1);

		++iter; ++iter; //2, 3
		REQUIRE(*iter == 3);

		--iter; --iter; --iter; //2, 1, 0
		REQUIRE(*iter == 0);

		--iter;
		REQUIRE(*iter == 0);

		++iter; //1
		REQUIRE(*iter == 1);

		++iter; ++iter; ++iter; --iter; //2, 3, !, 3
		REQUIRE(*iter == 3);
	}
}
