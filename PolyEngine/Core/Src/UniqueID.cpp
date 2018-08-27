#include "CorePCH.hpp"

#include "UniqueID.hpp"
#include "Math/Random.hpp"
#include "Utils/HexUtils.hpp"
#include "Collections/StringBuilder.hpp"

using namespace Poly;

const UniqueID UniqueID::INVALID;

UniqueID::UniqueID() 
{
	UUID.fill(0);
}

UniqueID UniqueID::Generate() 
{ 
	UniqueID ret;

	// 1. Generate 16 random bytes = 128 bits
	for (size_t i = 0; i < ret.UUID.size(); ++i)
		ret.UUID[i] = RandomRange(0x00, 0xFF);

	// 2. Adjust certain bits according to RFC 4122 section 4.4.
	// This just means do the following
	// (a) set the high nibble of the 7th byte equal to 4 and
	// (b) set the two most significant bits of the 9th byte to 10'B,
	//     so the high nibble will be one of {8,9,A,B}.
	ret.UUID[6] = 0x40 | (ret.UUID[6] & 0xf);
	ret.UUID[8] = 0x80 | (ret.UUID[8] & 0x3f);

	// Return the UUID string
	return ret;
}

bool UniqueID::operator==(const UniqueID& rhs) const 
{ 
	for (size_t i = 0; i < UUID.size(); ++i)
	{
		if (UUID[i] != rhs.UUID[i])
			return false;
	}
	return true; 
}
bool UniqueID::operator!=(const UniqueID& rhs) const { return !(*this == rhs); }

bool Poly::UniqueID::IsValid() const
{
	return *this != INVALID;
}

UniqueID::operator bool() const
{
	return IsValid();
}

size_t Poly::UniqueID::GetHash() const
{
	// @fixme(muniu) slow implementation, optimize
	std::stringstream ss;
	ss << *this;
	return std::hash<std::string>{}(ss.str());
}

String Poly::UniqueID::ToString() const
{
	StringBuilder sb;
	sb.Append(*this);
	return sb.StealString();
}

Optional<UniqueID> Poly::UniqueID::FromString(const String& str)
{
	if (str.GetLength() != 36)
		return {};
	
	UniqueID uuid;
	size_t count = 0;
	for (size_t i = 0; i < str.GetLength(); i += 2)
	{
		if (str[i] == '-') ++i;
		auto a = str[i];
		if (str[i + 1] == '-') ++i;
		auto b = str[i + 1];

		if(!IsValidHex(a) || !IsValidHex(b))
			return {};

		uuid.UUID[count] = (HexCharToValue(a) << 4) | HexCharToValue(b);
		++count;
	}
	
	HEAVY_ASSERTE(str.ToLower() == uuid.ToString(), "");
	return uuid;
}


namespace Poly {
	#define HEX_PRINT(val) std::setfill('0') << std::setw(2) << std::hex << (u32)val

	//------------------------------------------------------------------------------
	std::ostream& operator<< (std::ostream& stream, const UniqueID& uuid) 
	{
		return stream
			<< HEX_PRINT(uuid.UUID[0]) << HEX_PRINT(uuid.UUID[1]) 
			<< HEX_PRINT(uuid.UUID[2]) << HEX_PRINT(uuid.UUID[3]) << "-"
			<< HEX_PRINT(uuid.UUID[4]) << HEX_PRINT(uuid.UUID[5]) << "-"
			<< HEX_PRINT(uuid.UUID[6]) << HEX_PRINT(uuid.UUID[7]) << "-"
			<< HEX_PRINT(uuid.UUID[8]) << HEX_PRINT(uuid.UUID[9]) << "-"
			<< HEX_PRINT(uuid.UUID[10]) << HEX_PRINT(uuid.UUID[11]) 
			<< HEX_PRINT(uuid.UUID[12]) << HEX_PRINT(uuid.UUID[13]) 
			<< HEX_PRINT(uuid.UUID[14]) << HEX_PRINT(uuid.UUID[15]);
	}
}