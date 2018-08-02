#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

ASSIGN_CONTROL(StringControl, RTTI::eCorePropertyType::STRING, STRING)

//------------------------------------------------------------------------------
StringControl::StringControl(QWidget* parent)
	: ControlBase(parent)
{
	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Field = new QLineEdit();
	connect(Field, &QLineEdit::editingFinished, this, &StringControl::UpdateObject);

	Layout->addWidget(Field);

	setLayout(Layout);
}

//------------------------------------------------------------------------------
void StringControl::Reset()
{
	ControlBase<Poly::String>::Reset();

	Field->setText("");
}

//------------------------------------------------------------------------------
void StringControl::UpdateControl()
{
	if (Field->hasFocus())
		return;

	Field->setText(Object->GetCStr());
}

//------------------------------------------------------------------------------
void StringControl::UpdateObject()
{
	if (DisableEdit || *Object == Field->text().toLatin1().data())
		return;

	ControlCommand<String>* cmd = new ControlCommand<String>();
	cmd->Object = Object;
	cmd->Control = this;
	cmd->UndoValue = new String(*Object);
	cmd->RedoValue = new String(Field->text().toLatin1().data());

	*Object = Field->text().toLatin1().data();

	emit ObjectUpdated(cmd);
}
