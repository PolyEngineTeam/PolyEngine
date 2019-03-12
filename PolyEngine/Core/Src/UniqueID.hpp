#pragma once

#include <Defines.hpp>
#include <Utils/Optional.hpp>
#include <Collections/String.hpp>

namespace Poly
{
	class CORE_DLLEXPORT UniqueID final : public BaseObjectLiteralType<>
	{
	public:
		static const UniqueID INVALID;

		UniqueID();
		static UniqueID Generate();

		bool operator==(const UniqueID& rhs) const;
		bool operator!=(const UniqueID& rhs) const;

		bool IsValid() const;
		explicit operator bool() const;

		size_t GetHash() const;
		String ToString() const;
		static Optional<UniqueID> FromString(const String& str);
		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const UniqueID& uuid);
	private:
		// 128-bit RFC4122 compliant uuid
		std::array<unsigned char, 16> UUID;
	};
}

DECLARE_STD_HASHER(Poly::UniqueID)