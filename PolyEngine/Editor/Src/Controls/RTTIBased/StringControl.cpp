#include "PolyEditorPCH.hpp"

StringControl::StringControl(void* ptr, const RTTI::Property& prop, RTTI::eCorePropertyType)
	: Object(reinterpret_cast<String*>(ptr))
{
	InitializeControl();
}

void StringControl::UpdateObject()
{
	*Object = Field->text().toLatin1().data();
}

void StringControl::UpdateControl()
{
	Field->setText(Object->GetCStr());
}

void StringControl::InitializeControl()
{
	Field = std::make_unique<QLineEdit>(this);
	Field->setText(Object->GetCStr());
}
