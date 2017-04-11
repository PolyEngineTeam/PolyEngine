#pragma once

#include "Defines.hpp"

namespace Poly {

	//TODO write propper UniqueID
	class CORE_DLLEXPORT UniqueID
	{
	public:
		UniqueID();
		static UniqueID Generate();

		bool operator==(const UniqueID& rhs) const;
		bool operator!=(const UniqueID& rhs) const;

		operator bool() const;
	private:
		UniqueID(size_t id);

		size_t ID = 0;
	};
}