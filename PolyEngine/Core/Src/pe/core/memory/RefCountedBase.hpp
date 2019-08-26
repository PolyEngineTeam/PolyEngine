#pragma once

#include <pe/Defines.hpp>
#include <pe/core/BaseObject.hpp>

namespace pe::core::memory
{
	class CORE_DLLEXPORT RefCountedBase : public BaseObject<>
	{
	public:
		virtual ~RefCountedBase();

		void AddRef();
		bool RemoveRef();

		size_t GetRefCount() const { return RefCount; }

	private:
		size_t RefCount = 0;
	};
}