#pragma once

#include <unordered_map>

#include "Defines.hpp"
#include "Dynarray.hpp"

namespace Poly {
	class CORE_DLLEXPORT SafePtrRoot : public BaseObject<>
	{
	public:
		//TODO remove later
		int Debug();

		virtual ~SafePtrRoot();

		//shouldnt this methods be private, only available to SafePtr somehow?
		static size_t RegisterPointer(SafePtrRoot *pointer);
		static void ClearPointer(SafePtrRoot *pointer);
		static SafePtrRoot *GetPointer(size_t idx);

	private:
		static Dynarray<SafePtrRoot*> Pointers;
		static std::unordered_map<SafePtrRoot*, size_t> PointersMap;
	};
}