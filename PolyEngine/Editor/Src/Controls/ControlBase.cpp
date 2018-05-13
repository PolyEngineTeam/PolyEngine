#include "PolyEditorPCH.hpp"

namespace Impl
{
	ControlCreator* CoreTypeToControlMap = 0;

	void* ControlCreator::operator new(size_t, void* where)
	{
		static bool mapInitialized = false;

		if (!mapInitialized)
		{
			mapInitialized = true;

			size_t offset = ((size_t)where - (size_t)CoreTypeToControlMap) / sizeof(ControlCreator);

			CoreTypeToControlMap = new ControlCreator[static_cast<int>(RTTI::eCorePropertyType::_COUNT)];
			for (int i = 0; i < static_cast<int>(RTTI::eCorePropertyType::_COUNT); ++i)
				new(&CoreTypeToControlMap[i]) ControlCreator([]() -> ControlBase* { return new PlaceHolderControl(); });
		
			return CoreTypeToControlMap + offset;
		}

		return where;
	}
}

void ControlBase::SetObject(void* ptr, const RTTI::Property* prop)
{
	Object = ptr; 
	Property = prop; 
	InitializeControl(); 
}
