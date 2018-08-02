#include "PolyEditorPCH.hpp"

void ControlBase::SetObject(void * ptr, const RTTI::Property * prop)
{
	Object = ptr;
	Property = prop;
	UpdateControl();
	setToolTip(Property->Name.GetCStr());
}

void ControlBase::Reset()
{
	Object = nullptr; 
	DisableEdit = false; 
	Property = nullptr; 
	ASAPUpdate = true;
}

bool ControlBase::ContainsLabel()
{
	return false;
}

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

bool ControlBase::GetDisableEdit()
{
	return DisableEdit;
}
