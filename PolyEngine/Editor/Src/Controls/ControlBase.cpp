#include "PolyEditorPCH.hpp"

namespace Impl
{
	ControlCreator* CoreTypeToControlMap = nullptr;

	//------------------------------------------------------------------------------
	void* ControlCreator::operator new(size_t, void* where)
	{
		static bool mapInitialized = false;

		// if cntrol creator is created for the firt time, memory must be allocated for CoreTypeToControlMap
		if (!mapInitialized)
		{
			mapInitialized = true;

			// calculate offset so after map initialization we'll know where to put this newly created ControlCreator
			size_t offset = ((size_t)where - (size_t)CoreTypeToControlMap) / sizeof(ControlCreator);

			// allocate memory for CoreTypeToControlMap
			CoreTypeToControlMap = new ControlCreator[static_cast<int>(RTTI::eCorePropertyType::_COUNT)];

			// for each cell create control creator for placement control so if any type has no assigned control
			// it will display placeholder control
			for (int i = 0; i < static_cast<int>(RTTI::eCorePropertyType::_COUNT); ++i)
				new(&CoreTypeToControlMap[i]) ControlCreator([](QWidget* parent) -> ControlBase* 
					{ return new PlaceHolderControl(parent); });
		
			// return pointer to cell where we want to add new control creator
			return CoreTypeToControlMap + offset;
		}
		// if CoreTypeToControlMap is initialized then we don't have to change anything
		else
			return where;
	}
}

//------------------------------------------------------------------------------
void ControlBase::SetObject(void* ptr, const RTTI::Property* prop)
{
	Object = ptr; 
	Property = prop; 
	UpdateControl();
	setToolTip(Property->Name.GetCStr());
}

//------------------------------------------------------------------------------
void ControlBase::SetDisableEdit(bool disable)
{
	DisableEdit = disable;

	if (disable)
	{
		setDisabled(true);
	}
	else
	{
		setPalette(style()->standardPalette());

		setDisabled(false);
	}
}
