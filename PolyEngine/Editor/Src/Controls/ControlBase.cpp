#include "ControlBase.hpp"

namespace Impl
{
	ControlCreatorPtr* CoreTypeToControlMap = new ControlCreatorPtr[static_cast<int>(RTTI::eCorePropertyType::_COUNT)];
}