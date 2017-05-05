#pragma once

#include "Defines.hpp"

namespace Poly {

	enum class CORE_DLLEXPORT eBaseObjectType
	{
		DEFAULT,
		_COUNT
	};

	class CORE_DLLEXPORT CustomAlocatedObject
	{
	public:
		void* operator new(size_t size);
		void operator delete(void* ptr);
	};


	template<eBaseObjectType TYPE = eBaseObjectType::DEFAULT>
	class BaseObject : public CustomAlocatedObject
	{
	public:
		virtual ~BaseObject() {}
	};

	// Version without virtual destructor. Can be used only for constexpr classes and other PODs that prohibit virtual methods in base classes.
	// USE THIS ONLY WHEN YOU KNOW WHAT YOU'RE DOING!!!
	template<eBaseObjectType TYPE = eBaseObjectType::DEFAULT>
	class BaseObjectLiteralType : public CustomAlocatedObject
	{
	public:
	};

	template class CORE_DLLEXPORT BaseObject<eBaseObjectType::DEFAULT>;
	template class CORE_DLLEXPORT BaseObjectLiteralType<eBaseObjectType::DEFAULT>;
}