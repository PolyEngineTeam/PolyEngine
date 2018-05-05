#include "PolyEditorPCH.hpp"

DEFINE_CONTROL(StringControl, RTTI::eCorePropertyType::STRING)

StringControl::StringControl(QWidget* parent)
	: ControlBase(parent)
{
}

void StringControl::UpdateObject()
{
	*reinterpret_cast<String*>(Object) = Field->text().toLatin1().data();
}

void StringControl::UpdateControl()
{
	Field->setText(reinterpret_cast<String*>(Object)->GetCStr());
}

void StringControl::InitializeControl()
{
	Field = std::make_unique<QLineEdit>(this);
	Field->setText(reinterpret_cast<String*>(Object)->GetCStr());
}
