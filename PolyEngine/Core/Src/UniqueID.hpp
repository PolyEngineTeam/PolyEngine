#pragma once

#include "Defines.hpp"

namespace Poly {

	//TODO write propper UniqueID
	class CORE_DLLEXPORT UniqueID : public BaseObject<>
	{
	public:
		UniqueID();
		static UniqueID Generate();

		bool operator==(const UniqueID& rhs) const;
		bool operator!=(const UniqueID& rhs) const;

		operator bool() const;

		size_t GetHash() const { return ID; }
	private:
		UniqueID(size_t id);

		size_t ID = 0;
	};
}

// hasher for UniqueID
namespace std {
	template <> struct hash<Poly::UniqueID> { std::size_t operator()(const Poly::UniqueID& k) const { return k.GetHash(); } };
}