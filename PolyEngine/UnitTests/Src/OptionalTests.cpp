#include <catch.hpp>

#include <Optional.hpp>

using namespace Poly;

TEST_CASE("Optional with default-constructible", "[Optional]") {
	SECTION("Value (empty)") {
		const Optional<char> opt{};
		REQUIRE_FALSE(opt.HasValue());
	}

	SECTION("Reference (empty)") {
		const Optional<char&> opt{};
		REQUIRE_FALSE(opt.HasValue());
	}

	SECTION("Const reference (empty)") {
		const Optional<const char&> opt{};
		REQUIRE_FALSE(opt.HasValue());
	}

	SECTION("Value") {
		const Optional<char> opt{8};
		REQUIRE(opt.HasValue());
		REQUIRE(opt.Value() == 8);
	}

	SECTION("Reference") {
		char val = 8;
		const Optional<char&> opt{val};
		REQUIRE(opt.HasValue());
		REQUIRE(opt.Value() == 8);
	}

	SECTION("Const reference") {
		const char val = 8;
		const Optional<const char&> opt{val};
		REQUIRE(opt.HasValue());
		REQUIRE(opt.Value() == 8);
	}

	SECTION("ValueOr()") {
		REQUIRE(Optional<char>{}.ValueOr(16) == 16);
		REQUIRE(Optional<char>{8}.ValueOr(16) == 8);
	}

	SECTION("Taking away value") {
		Optional<char> opt{8};
		const char taken = opt.TakeValue();
		REQUIRE_FALSE(opt.HasValue());
		REQUIRE(taken == 8);
	}
}

TEST_CASE("Optional with non-default-constructible", "[Optional]") {
	static size_t gCurrentInstances = 0; //counts living object at the given time
	static size_t gCopiesEver = 0; //never goes down unless set manually
	static size_t gMovesEver  = 0; //^
	struct Counting {
		Counting(char value) : value(value) { gCurrentInstances += 1; }
		Counting(const Counting&  other) : Counting(other.value)            { gCopiesEver += 1; }
		Counting(      Counting&& other) : Counting(std::move(other.value)) { gMovesEver += 1; }
		~Counting() { gCurrentInstances -= 1; }
		Counting& operator=(const Counting&  other) { this->value = other.value; gCopiesEver += 1; return *this; }
		Counting& operator=(      Counting&& other) { this->value = other.value; gMovesEver  += 1; return *this; }
		bool operator==(const Counting& other) const { return this->value == other.value; }
		char value;
	};

	SECTION("Do not construct objects in empty optionals") {
		const Optional<Counting> opt{};
		REQUIRE_FALSE(opt.HasValue());
		REQUIRE(gCurrentInstances == 0);
	}

	SECTION("Avoid copying value around") {
		REQUIRE(gCurrentInstances == 0);
		gCopiesEver = 0;
		gMovesEver  = 0;
		{
			const Optional<Counting> opt{{8}};
			REQUIRE(opt.HasValue());
			REQUIRE(gCurrentInstances == 1);
			REQUIRE(opt.Value() == Counting(8));
			const Counting& possiblyBindingRef = opt.Value();
			REQUIRE(gCurrentInstances == 1);
		}
		REQUIRE(gCurrentInstances == 0);
		REQUIRE(gCopiesEver == 0);
		REQUIRE(gMovesEver  == 1); //one into the optional
	}

	SECTION("Do not copy when taking value by reference") {
		REQUIRE(gCurrentInstances == 0);
		Counting val(8);
		Optional<Counting&> opt{val};
		(void) opt; //silence the warning
		REQUIRE(gCurrentInstances == 1);
	}

	SECTION("Avoid copying when taking away") {
		REQUIRE(gCurrentInstances == 0);
		gCopiesEver = 0;
		gMovesEver  = 0;
		{
			Optional<Counting> opt{{8}};
			Counting taken = opt.TakeValue();
			REQUIRE(gCurrentInstances == 1);
		}
		REQUIRE(gCurrentInstances == 0);
		REQUIRE(gCopiesEver == 0);
		REQUIRE(gMovesEver  == 2); //one into the optional, one out
	}

	SECTION("Avoid copying during ValueOr()") {
		REQUIRE(gCurrentInstances == 0);
		gCopiesEver = 0;
		gMovesEver  = 0;
		{
			const Optional<Counting> none{};
			const Optional<Counting> some{{8}};
			REQUIRE(none.ValueOr(Counting(16)) == Counting(16));
			REQUIRE(some.ValueOr(Counting(16)) == Counting(8));
		}
		REQUIRE(gCurrentInstances == 0);
		REQUIRE(gCopiesEver == 2); //todo(vuko): implementation of ValueOr() was based on the official C++17 optional; can we do better?
		REQUIRE(gMovesEver  == 1); //one into the optional
	}

	SECTION("Move constructor") {
		gCopiesEver = 0;
		gMovesEver = 0;
		Optional<Counting> a{{8}};
		Optional<Counting> b{std::move(a)};
		REQUIRE_FALSE(a.HasValue());
		REQUIRE(b.HasValue());
		REQUIRE(gCopiesEver == 0);
		REQUIRE(gMovesEver  == 2); //one into, one from a to b
	}

	SECTION("Move assignment") {
		gCopiesEver = 0;
		gMovesEver = 0;
		Optional<Counting> a{{8}};
		Optional<Counting> b = std::move(a);
		REQUIRE_FALSE(a.HasValue());
		REQUIRE(b.HasValue());
		REQUIRE(gCopiesEver == 0);
		REQUIRE(gMovesEver  == 2); //one into, one from a to b
	}
}
