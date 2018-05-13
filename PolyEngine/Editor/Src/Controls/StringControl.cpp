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
	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Field = new QLineEdit(this);
	Field->setText(reinterpret_cast<String*>(Object)->GetCStr());

	Layout->addWidget(Field);

	setLayout(Layout);
}
