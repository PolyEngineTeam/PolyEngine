#pragma once

#include <unordered_map>

#include "Defines.hpp"
#include "Dynarray.hpp"
#include "RTTI.hpp"

namespace Poly {
	class CORE_DLLEXPORT SafePtrRoot : public RTTIBase
	{
	public:
		static size_t RegisterPointer(SafePtrRoot *pointer);
		static void ClearPointer(SafePtrRoot *pointer);
		static SafePtrRoot *GetPointer(size_t idx);

		virtual ~SafePtrRoot();

	private:

		static Dynarray<SafePtrRoot*> Pointers;
		static std::unordered_map<SafePtrRoot*, size_t> PointersMap;
	};
}