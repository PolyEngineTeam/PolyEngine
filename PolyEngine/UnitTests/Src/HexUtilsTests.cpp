#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/utils/HexUtils.hpp>

TEST_CASE("Hex to char", "[HexUtils]") 
{
	CHECK(::pe::core::utils::HexCharToValue('0') == 0x00);
	CHECK(::pe::core::utils::HexCharToValue('1') == 0x01);
	CHECK(::pe::core::utils::HexCharToValue('2') == 0x02);
	CHECK(::pe::core::utils::HexCharToValue('3') == 0x03);
	CHECK(::pe::core::utils::HexCharToValue('4') == 0x04);
	CHECK(::pe::core::utils::HexCharToValue('5') == 0x05);
	CHECK(::pe::core::utils::HexCharToValue('6') == 0x06);
	CHECK(::pe::core::utils::HexCharToValue('7') == 0x07);
	CHECK(::pe::core::utils::HexCharToValue('8') == 0x08);
	CHECK(::pe::core::utils::HexCharToValue('9') == 0x09);

	CHECK(::pe::core::utils::HexCharToValue('a') == 0x0A);
	CHECK(::pe::core::utils::HexCharToValue('b') == 0x0B);
	CHECK(::pe::core::utils::HexCharToValue('c') == 0x0C);
	CHECK(::pe::core::utils::HexCharToValue('d') == 0x0D);
	CHECK(::pe::core::utils::HexCharToValue('e') == 0x0E);
	CHECK(::pe::core::utils::HexCharToValue('f') == 0x0F);

	CHECK(::pe::core::utils::HexCharToValue('A') == 0x0A);
	CHECK(::pe::core::utils::HexCharToValue('B') == 0x0B);
	CHECK(::pe::core::utils::HexCharToValue('C') == 0x0C);
	CHECK(::pe::core::utils::HexCharToValue('D') == 0x0D);
	CHECK(::pe::core::utils::HexCharToValue('E') == 0x0E);
	CHECK(::pe::core::utils::HexCharToValue('F') == 0x0F);
}
