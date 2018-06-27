#include "PolyEditorPCH.hpp"

ASSIGN_CONTROL(StringControl, RTTI::eCorePropertyType::STRING, STRING)

//------------------------------------------------------------------------------
StringControl::StringControl(QWidget* parent)
	: ControlBase(parent)
{
	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Field = new QLineEdit();
	connect(Field, &QLineEdit::editingFinished, this, &ControlBase::Confirm);

	Layout->addWidget(Field);

	setLayout(Layout);
}

//------------------------------------------------------------------------------
void StringControl::Reset()
{
	Object = nullptr;

	Field->setText("");
}

//------------------------------------------------------------------------------
void StringControl::UpdateObject()
{
	*reinterpret_cast<String*>(Object) = Field->text().toLatin1().data();
}

//------------------------------------------------------------------------------
void StringControl::UpdateControl()
{
	Field->setText(reinterpret_cast<String*>(Object)->GetCStr());
}
