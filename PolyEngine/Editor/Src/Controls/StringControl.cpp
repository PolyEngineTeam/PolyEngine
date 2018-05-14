#include "PolyEditorPCH.hpp"

ASSIGN_CONTROL(StringControl, STRING)

StringControl::StringControl()
{
	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Field = new QLineEdit();

	Layout->addWidget(Field);

	setLayout(Layout);
	setToolTip("STRING");
}

void StringControl::UpdateObject()
{
	*reinterpret_cast<String*>(Object) = Field->text().toLatin1().data();
}

void StringControl::UpdateControl()
{
	Field->setText(reinterpret_cast<String*>(Object)->GetCStr());
}
