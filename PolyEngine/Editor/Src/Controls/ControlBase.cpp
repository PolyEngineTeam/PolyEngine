#include "ControlBase.hpp"

namespace Impl
{
	ControlCreator** CoreTypeToControlMap = new ControlCreator*[static_cast<int>(RTTI::eCorePropertyType::_COUNT)];

	void* ControlCreator::operator new(size_t, void* where)
	{
		static bool mapInitialized = false;

		if (!mapInitialized)
		{
			CoreTypeToControlMap = new ControlCreator*[static_cast<int>(RTTI::eCorePropertyType::_COUNT)];
			mapInitialized = true;
		}

		return where;
	}
}