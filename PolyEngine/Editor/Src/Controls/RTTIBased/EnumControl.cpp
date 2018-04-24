#include "PolyEditorPCH.hpp"

#include <QComboBox>

EnumControl::EnumControl(void* ptr, const RTTI::Property& prop, RTTI::eCorePropertyType)
{
	//Object = ptr;
	
	//ComboBox = std::make_unique<QComboBox>(this);
	
	//HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid enum impl data!");
	//const RTTI::EnumPropertyImplData* implData = static_cast<const RTTI::EnumPropertyImplData*>(prop.ImplData.get());
	//i64 val;
	//if (implData->EnumInfo->GetUnderlyingValueSize() == sizeof(i32))
	//	val = *reinterpret_cast<const i32*>(ptr);
	//else if (implData->EnumInfo->GetUnderlyingValueSize() == sizeof(i64))
	//	val = *reinterpret_cast<const i64*>(ptr);
	//else
	//	ASSERTE(false, "Unhadled value size!");
	//
	//v = implData->EnumInfo->GetEnumName(val);
}

void EnumControl::UpdateObject()
{
}

void EnumControl::UpdateControl()
{
}
