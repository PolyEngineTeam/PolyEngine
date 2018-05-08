#include <catch.hpp>

#include <Collections/BitMask.hpp>

using namespace Poly;

constexpr BitMask::DataType TYPE_BIT = CHAR_BIT * sizeof(BitMask::DataType);

//Function calculating expected Dynarray size ,for debugging purposes 
size_t DynarraySize(BitMask mask)
{
	if (mask.GetSize() % TYPE_BIT)
		return mask.GetSize() / TYPE_BIT + 1;
	else
		return mask.GetSize() / TYPE_BIT;
}

TEST_CASE("BitMask constructors", "[BitMask]")
{
	//Default constructor
	BitMask a;
	REQUIRE(a.GetSize() == 1 * sizeof(BitMask::DataType));
	REQUIRE(a.GetDynarraySize() == DynarraySize(a));
	BitMask b;
	REQUIRE(a.GetSize() == b.GetSize());
	REQUIRE(a.GetDynarraySize() == b.GetDynarraySize());
	//"Not even" constructor
	BitMask c(20);
	REQUIRE(c.GetSize() == 20);
	REQUIRE(c.GetDynarraySize() == DynarraySize(c));
	for (size_t i = 0; i < a.GetSize(); i++)
		REQUIRE(a[i] == false);
	for (size_t i = 0; i < b.GetSize(); i++)
		REQUIRE(b[i] == false);
	for (size_t i = 0; i < c.GetSize(); i++)
		REQUIRE(c[i] == false);
	//Even constructor
	BitMask d(16);
	REQUIRE(d.GetSize() == 16);
	REQUIRE(d.GetDynarraySize() == DynarraySize(d));
}

TEST_CASE("BitMask Set,Toggle and Reset", "[BitMask]")
{
	BitMask a(8);
	//Set method
	a.Set(7, true);
	a.Set(0, true);
	CHECK(a[7] == true);
	CHECK(a[0] == true);
	a.Set(7, false);
	a.Set(0, false);
	CHECK(a[7] == false);
	CHECK(a[0] == false);

	//Toggle method
	a.Toggle(1);
	CHECK(a[1] == true);
	a.Toggle(1);
	CHECK(a[1] == false);

	//Reset method
	a.Reset();
	CHECK(a.GetSize() == 0);
	CHECK(a.GetDynarraySize() == DynarraySize(a));
}
TEST_CASE("Bitwise operators", "[BitMask]")
{
	SECTION("Bitwise OR operator")
	{
		SECTION("Equal Dynarray sizes")
		{
			BitMask a(17), b(22), c;
			a.Toggle(0); a.Toggle(3); a.Toggle(10); a.Toggle(15);
			b.Toggle(0); b.Toggle(5); b.Toggle(11); b.Toggle(20);
			c = a | b;
			CHECK(c.GetSize() == 22);
			CHECK(c.GetDynarraySize() == DynarraySize(c));
			for (size_t i = 0; i < c.GetSize(); i++)
			{
				if (i == 0 || i == 3 || i == 5 || i == 10 ||
					i == 11 || i == 15 || i == 20)
					CHECK(c[i] == true);
				else
					CHECK(c[i] == false);
			}
		}
		SECTION("Different  Dynarray sizes")
		{
			BitMask a(16), b(5), c;
			b.Toggle(1); b.Toggle(2); b.Toggle(4);
			a.Toggle(0); a.Toggle(1); a.Toggle(8); a.Toggle(10); a.Toggle(15);
			BitMask *left, *right;
			left = &a; right = &b;
			SECTION("Lhs size bigger than rhs size")
			{
				c = *left | *right;
				CHECK(c.GetSize() == 16);
				CHECK(c.GetDynarraySize() == DynarraySize(c));
				for (size_t i = 0; i < c.GetSize(); i++)
				{
					if (i == 0 || i == 1 || i == 2 || i == 4 ||
						i == 8 || i == 15 || i == 10)
						CHECK(c[i] == true);
					else
						CHECK(c[i] == false);
				}
			}
			SECTION("Rhs size bigger than lhs size")
			{
				left = &b; right = &a;
				BitMask d = *left | *right;
				CHECK(d.GetSize() == 16);
				CHECK(d.GetDynarraySize() == DynarraySize(d));
				for (size_t i = 0; i < d.GetSize(); i++)
				{
					if (i == 0 || i == 1 || i == 2 || i == 4 ||
						i == 8 || i == 15 || i == 10)
						CHECK(d[i] == true);
					else
						CHECK(d[i] == false);
				}
			}
		}
	}
	SECTION("Bitwise XOR operator")
	{
		SECTION("Equal Dynarray sizes")
		{
			BitMask a(62), b(57), c;
			a.Toggle(4); a.Toggle(21); a.Toggle(29); a.Toggle(59);
			b.Toggle(4); b.Toggle(18); b.Toggle(21); b.Toggle(29); b.Toggle(55);
			c = a ^ b;
			CHECK(c.GetSize() == 62);
			CHECK(c.GetDynarraySize() == DynarraySize(c));
			for (size_t i = 0; i < c.GetSize(); i++)
				if (i == 18 || i == 59 || i == 55)
					CHECK(c[i] == true);
				else
					CHECK(c[i] == false);
		}
		SECTION("Different Dynarray sizes")
		{
			BitMask a(4), b(25);
			a.Toggle(0); a.Toggle(2); a.Toggle(3);
			b.Toggle(0); b.Toggle(2); b.Toggle(10); b.Toggle(20);
			BitMask *left = &a, *right = &b;
			SECTION("Rhs size bigger than lhs size")
			{
				BitMask c = *left ^ *right;
				CHECK(c.GetSize() == 25);
				CHECK(c.GetDynarraySize() == DynarraySize(c));
				for (size_t i = 0; i < c.GetSize(); i++)
					if (i == 3 || i == 10 || i == 20)
						CHECK(c[i] == true);
					else
						CHECK(c[i] == false);
			}
			SECTION("Lhs size bigger than rhs size")
			{
				left = &b; right = &a;
				BitMask c = *left ^ *right;
				CHECK(c.GetSize() == 25);
				CHECK(c.GetDynarraySize() == DynarraySize(c));
				for (size_t i = 0; i < c.GetSize(); i++)
					if (i == 3 || i == 10 || i == 20)
						CHECK(c[i] == true);
					else
						CHECK(c[i] == false);
			}
		}
	}
	SECTION("Bitwise AND operator")
	{
		SECTION("Equal Dynarray sizes")
		{
			BitMask a(12), b(14);
			a.Toggle(0); a.Toggle(7); a.Toggle(8); a.Toggle(11);
			b.Toggle(0); b.Toggle(1); b.Toggle(7); b.Toggle(9); b.Toggle(13);
			BitMask c = a&b;
			CHECK(c.GetSize() == 14);
			CHECK(c.GetDynarraySize() == DynarraySize(c));
			for (size_t i = 0; i < c.GetSize(); i++)
			{
				if (i == 0 || i == 7)
					CHECK(c[i] == true);
				else
					CHECK(c[i] == false);
			}
			SECTION("Different Dynarray sizes")
			{
				BitMask a(21), b(8);
				a.Toggle(1); a.Toggle(5); a.Toggle(13); a.Toggle(14); a.Toggle(19);
				b.Toggle(1); b.Toggle(5); b.Toggle(7);
				BitMask *left, *right;
				SECTION("Rhs size bigger than lhs size")
				{
					left = &b; right = &a;
					BitMask c = *left & *right;
					CHECK(c.GetSize() == 21);
					CHECK(c.GetDynarraySize() == DynarraySize(c));
					for (size_t i = 0; i < c.GetSize(); i++)
						if (i == 1 || i == 5)
							CHECK(c[i] == true);
						else
							CHECK(c[i] == false);
				}
				SECTION("Lhs size bigger than rhs size")
				{
					left = &a; right = &b;
					BitMask c = *left & *right;
					CHECK(c.GetSize() == 21);
					CHECK(c.GetDynarraySize() == DynarraySize(c));
					for (size_t i = 0; i < c.GetSize(); i++)
						if (i == 1 || i == 5)
							CHECK(c[i] == true);
						else
							CHECK(c[i] == false);
				}
			}
		}
	}
}

TEST_CASE("Negate operator", "[BitMask]")
{
	BitMask a(10);
	for (size_t i = 0; i < a.GetSize(); i++)
		if (i % 2) //Toggle for odd numbers
			a.Toggle(i);

	~a;
	for (size_t i = 0; i < a.GetSize(); i++)
		if (i % 2)
			CHECK(a[i] == false);
		else
			CHECK(a[i] == true);
}

TEST_CASE("Bitwise assignment operators", "[BitMask]")
{
	SECTION("Bitwise OR assignment operator")
	{
		SECTION("Equal Dynarray sizes")
		{
			BitMask a(13), b(10);
			a.Toggle(0); a.Toggle(5); a.Toggle(7); a.Toggle(10);
			b.Toggle(0); b.Toggle(4); b.Toggle(5);
			a |= b;
			CHECK(a.GetSize() == 13);
			CHECK(a.GetDynarraySize() == DynarraySize(a));
			for (size_t i = 0; i < a.GetSize(); i++)
			{
				if (i == 0 || i == 4 || i == 5 || i == 7 || i == 10)
					CHECK(a[i] == true);
				else
					CHECK(a[i] == false);
			}
		}
		SECTION("Different Dynarray sizes")
		{
			SECTION("Lhs size bigger than rhs size")
			{
				BitMask a(7), b(26);
				a.Toggle(0); a.Toggle(2); a.Toggle(4); a.Toggle(6);
				b.Toggle(0); b.Toggle(3); b.Toggle(15); b.Toggle(21);

				b |= a;
				CHECK(b.GetSize() == 26);
				CHECK(b.GetDynarraySize() == DynarraySize(b));
				for (size_t i = 0; i < b.GetSize(); i++)
					if (i == 0 || i == 2 || i == 3 || i == 4 || i == 6 || i == 15 || i == 21)
						CHECK(b[i] == true);
					else
						CHECK(b[i] == false);
			}

			SECTION("Rhs size bigger than lhs size")
			{
				BitMask a(7), b(26);
				a.Toggle(0); a.Toggle(2); a.Toggle(4); a.Toggle(6);
				b.Toggle(0); b.Toggle(3); b.Toggle(15); b.Toggle(21);

				a |= b;
				CHECK(a.GetSize() == 26);
				CHECK(a.GetDynarraySize() == DynarraySize(a));
				for (size_t i = 0; i < a.GetSize(); i++)
					if (i == 0 || i == 2 || i == 3 || i == 4 || i == 6 || i == 15 || i == 21)
						CHECK(a[i] == true);
					else
						CHECK(a[i] == false);
			}
		}
	}

	SECTION("Bitwise AND assignment operator")
	{
		SECTION("Equal dynarray sizes")
		{
			BitMask a(13), b(10);
			a.Toggle(0); a.Toggle(3); a.Toggle(6); a.Toggle(12); a.Toggle(9);
			b.Toggle(0); b.Toggle(3); b.Toggle(5); b.Toggle(9);
			a &= b;
			CHECK(a.GetSize() == 13);
			CHECK(a.GetDynarraySize() == DynarraySize(a));
			for (size_t i = 0; i < a.GetSize(); i++)
			{
				if (i == 0 || i == 3 || i == 9)
					CHECK(a[i] == true);
				else
					CHECK(a[i] == false);
			}
		}
		SECTION("Different Dynarray sizes")
		{
			SECTION("Rhs dynarray size bigger than lhs size")
			{
				BitMask a(4), b(23);
				a.Toggle(1); a.Toggle(2); a.Toggle(3);
				b.Toggle(2); b.Toggle(3); b.Toggle(10); b.Toggle(20); b.Toggle(15);
				a &= b;
				CHECK(a.GetSize() == 23);
				CHECK(a.GetDynarraySize() == DynarraySize(a));
				for (size_t i = 0; i < a.GetSize(); i++)
					if (i == 2 || i == 3)
						CHECK(a[i] == true);
					else
						CHECK(a[i] == false);
			}
			SECTION("Lhs dynarray size bigger than rhs size")
			{
				BitMask a(4), b(23);
				a.Toggle(1); a.Toggle(2); a.Toggle(3);
				b.Toggle(2); b.Toggle(3); b.Toggle(10); b.Toggle(20); b.Toggle(15);
				b &= a;
				CHECK(b.GetSize() == 23);
				CHECK(b.GetDynarraySize() == DynarraySize(b));
				for (size_t i = 0; i < b.GetSize(); i++)
					if (i == 2 || i == 3)
						CHECK(b[i] == true);
					else
						CHECK(b[i] == false);
			}
		}
	}
	SECTION("Bitwise XOR assignment operator")
	{
		SECTION("Same dynarray sizes")
		{
			BitMask a(29), b(31);
			a.Toggle(3); a.Toggle(7); a.Toggle(17); a.Toggle(23); a.Toggle(26);
			b.Toggle(7); b.Toggle(15); b.Toggle(23); b.Toggle(26); b.Toggle(29);
			a ^= b;
			CHECK(a.GetSize() == 31);
			CHECK(a.GetDynarraySize() == DynarraySize(a));
			for (size_t i = 0; i < a.GetSize(); i++)
				if (i == 3 || i == 15 || i == 17 || i == 3 || i == 29)
					CHECK(a[i] == true);
				else
					CHECK(a[i] == false);
		}
		SECTION("Different Dynarray sizes")
		{
			SECTION("Rhs dynnaray size bigger than lhs")
			{
				BitMask a(8), b(16);
				a.Toggle(0); a.Toggle(3); a.Toggle(7);
				b.Toggle(1); b.Toggle(3); b.Toggle(7); b.Toggle(12); b.Toggle(15);
				a ^= b;
				CHECK(a.GetSize() == 16);
				CHECK(a.GetDynarraySize() == DynarraySize(a));
				for (size_t i = 0; i < a.GetSize(); i++)
					if (i == 0 || i == 1 || i == 12 || i == 15)
						CHECK(a[i] == true);
					else
						CHECK(a[i] == false);
			}
			SECTION("Lhs dynnaray size bigger than rhs")
			{
				BitMask a(8), b(16);
				a.Toggle(0); a.Toggle(3); a.Toggle(7);
				b.Toggle(1); b.Toggle(3); b.Toggle(7); b.Toggle(12); b.Toggle(15);
				b ^= a;
				CHECK(b.GetSize() == 16);
				CHECK(b.GetDynarraySize() == DynarraySize(b));

				for (size_t i = 0; i < b.GetSize(); i++)
					if (i == 0 || i == 1 || i == 12 || i == 15)
						CHECK(b[i] == true);
					else
						CHECK(b[i] == false);
			}
		}
	}
}

TEST_CASE("Comparision operator", "[BitMask]")
{
	SECTION("Different BitsNumbers")
	{
		BitMask a(4), b(5);
		CHECK_FALSE(a == b);
	}
	SECTION("Equal BitsNumbers")
	{
		BitMask a(5), b(5);
		a.Toggle(1);	a.Toggle(3);
		b.Toggle(1);	b.Toggle(3);
		CHECK(a == b);
		BitMask c = b;
		CHECK(c == b);
		CHECK(c == a);

		a.Toggle(2);
		CHECK_FALSE(a == b);
		CHECK_FALSE(a == c);
	}
}

TEST_CASE("Negation operator", "[BitMask]")
{
	BitMask a(5), b(6);
	CHECK(a != b);

	BitMask c(23), d(23);
	c.Toggle(21);
	d.Toggle(4);

	CHECK(c != d);
}

TEST_CASE("Resize function", "[BitMask]")
{
	BitMask a(14);
	CHECK(a.GetSize() == 14);
	CHECK(a.GetDynarraySize() == DynarraySize(a));

	a.Resize(6);
	CHECK(a.GetSize() == 20);
	CHECK(a.GetDynarraySize() == DynarraySize(a));

	a.Resize(-2);
	CHECK(a.GetSize() == 18);
	CHECK(a.GetDynarraySize() == DynarraySize(a));

	a.Resize(-4);
	CHECK(a.GetSize() == 14);
	CHECK(a.GetDynarraySize() == DynarraySize(a));

	a.Resize(-8);
	CHECK(a.GetSize() == 6);
	CHECK(a.GetDynarraySize() == DynarraySize(a));

	a.Resize(40);
	CHECK(a.GetSize() == 46);
	CHECK(a.GetDynarraySize() == DynarraySize(a));

	a.Resize(-6);
	CHECK(a.GetSize() == 40);
	CHECK(a.GetDynarraySize() == DynarraySize(a));

	a.Resize(-1);
	CHECK(a.GetSize() == 39);
	CHECK(a.GetDynarraySize() == DynarraySize(a));

	a.Resize(-33);
	CHECK(a.GetSize() == 6);
	CHECK(a.GetDynarraySize() == DynarraySize(a));

	a.Resize(-5);
	CHECK(a.GetSize() == 1);
	CHECK(a.GetDynarraySize() == DynarraySize(a));

	a.Resize(-1);
	CHECK(a.GetSize() == 0);
	CHECK(a.GetDynarraySize() == DynarraySize(a));
}