#pragma once

#include "Defines.hpp"

namespace Poly {

	enum class CORE_DLLEXPORT eAllocationStrategy
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

	/// <summary>
	/// This class is base for almost every object in the engine. It ensures that all memory management is done via custom allocators.
	/// <para>For safety reasons BaseObject introduces virtual destructor, to ensure that no memory leaks are ever introduced.
	/// If this is unaceptable, please refere to <see cref="BaseObjectLiteralType"/>.</para>
	/// <para>BaseObject can be templated to allow different allocation strategies.</para>
	/// </summary>
	template<eAllocationStrategy TYPE = eAllocationStrategy::DEFAULT>
	class BaseObject : public CustomAlocatedObject
	{
	public:
		virtual ~BaseObject() {}
	};

	/// <summary>
	/// This class is variation of <see cref="BaseObject"/> that does not introduce virtual destructor.
	/// <para>It should be used only for constexpr classes and other PODs that do not require virtual destructors.</para>
	/// <para>BaseObjectLiteralType can be templated to allow different allocation strategies.</para>
	/// <para>USE THIS ONLY WHEN YOU KNOW WHAT YOU'RE DOING!!!</para>
	/// </summary>
	template<eAllocationStrategy TYPE = eAllocationStrategy::DEFAULT>
	class BaseObjectLiteralType : public CustomAlocatedObject
	{};

	template class CORE_DLLEXPORT BaseObject<eAllocationStrategy::DEFAULT>;
	template class CORE_DLLEXPORT BaseObjectLiteralType<eAllocationStrategy::DEFAULT>;
}