#include <Defines.hpp>
#include <catch.hpp>

#include <Utils/HexUtils.hpp>

using namespace Poly;

TEST_CASE("Hex to char", "[HexUtils]") 
{
	CHECK(HexCharToValue('0') == 0x00);
	CHECK(HexCharToValue('1') == 0x01);
	CHECK(HexCharToValue('2') == 0x02);
	CHECK(HexCharToValue('3') == 0x03);
	CHECK(HexCharToValue('4') == 0x04);
	CHECK(HexCharToValue('5') == 0x05);
	CHECK(HexCharToValue('6') == 0x06);
	CHECK(HexCharToValue('7') == 0x07);
	CHECK(HexCharToValue('8') == 0x08);
	CHECK(HexCharToValue('9') == 0x09);

	CHECK(HexCharToValue('a') == 0x0A);
	CHECK(HexCharToValue('b') == 0x0B);
	CHECK(HexCharToValue('c') == 0x0C);
	CHECK(HexCharToValue('d') == 0x0D);
	CHECK(HexCharToValue('e') == 0x0E);
	CHECK(HexCharToValue('f') == 0x0F);

	CHECK(HexCharToValue('A') == 0x0A);
	CHECK(HexCharToValue('B') == 0x0B);
	CHECK(HexCharToValue('C') == 0x0C);
	CHECK(HexCharToValue('D') == 0x0D);
	CHECK(HexCharToValue('E') == 0x0E);
	CHECK(HexCharToValue('F') == 0x0F);
}
