#pragma once

#include <Defines.hpp>
#include <BaseObject.hpp>

namespace Poly
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