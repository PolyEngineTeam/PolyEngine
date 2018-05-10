#include "PolyEditorPCH.hpp"

ASSIGN_CONTROL(StringControl, STRING)

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
	Layout = std::make_unique<QGridLayout>();
	setLayout(Layout.get());

	Field = std::make_unique<QLineEdit>(this);
	Field->setText(reinterpret_cast<String*>(Object)->GetCStr());

	Layout->addWidget(Field.get());
}
