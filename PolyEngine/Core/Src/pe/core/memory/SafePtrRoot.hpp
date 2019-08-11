#pragma once

#include <pe/Defines.hpp>
#include <pe/core/rtti/RTTI.hpp>

namespace pe::core::memory
{
	/// <summary>Base class for all objects which require safe pointer</summary>
	class CORE_DLLEXPORT SafePtrRoot : public RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(::Poly::SafePtrRoot, ::Poly::RTTIBase) 
		{ NO_RTTI_PROPERTY(); }
	public:
		/// <summary>Registers given pointer</summary>
		/// <param name="pointer">Pointer to be registered</param>
		/// <returns>ID of registered pointer</returns>
		static size_t RegisterPointer(SafePtrRoot *pointer);


		/// <summary>Gets pointer with given ID</summary>
		/// <param name="idx">ID of requested pointer</param>
		/// <returns>Pointer stored at given ID</returns>
		static SafePtrRoot *GetPointer(size_t idx);

		virtual ~SafePtrRoot();

	private:
		static void ClearPointer(SafePtrRoot *pointer);

		static std::vector<SafePtrRoot*> Pointers;
		static std::unordered_map<SafePtrRoot*, size_t> PointersMap;
	};
}