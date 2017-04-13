#pragma once

#include "Defines.hpp"

namespace Poly {

	enum class CORE_DLLEXPORT eBaseObjectType
	{
		DEFAULT,
		_COUNT
	};

	template<eBaseObjectType ObjType = eBaseObjectType::DEFAULT>
	class BaseObject
	{
	public:
		void* operator new(size_t size)
		{
			//TODO use custom allocator
			void* mem = malloc(size);
			HEAVY_ASSERTE(mem, "Couldn't allocate memory!");
			return mem;
		}

		void operator delete(void* ptr)
		{
			//TODO use custom allocator
			HEAVY_ASSERTE(ptr, "");
			free(ptr);
		}

		virtual ~BaseObject() {}
	};

	// Version without virtual destructor. Can be used only for constexpr classes and other PODs that prohibit virtual methods in base classes.
	// USE THIS ONLY WHEN YOU KNOW WHAT YOU'RE DOING!!!
	template<eBaseObjectType ObjType = eBaseObjectType::DEFAULT>
	class CORE_DLLEXPORT BaseObjectLiteralType
	{
	public:
		void* operator new(size_t size)
		{
			//TODO use custom allocator
			void* mem = malloc(size);
			HEAVY_ASSERTE(mem, "Couldn't allocate memory!");
			return mem;
		}

		void operator delete(void* ptr)
		{
			//TODO use custom allocator
			HEAVY_ASSERTE(ptr, "");
			free(ptr);
		}
	};

}