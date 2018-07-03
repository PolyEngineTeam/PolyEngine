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

//------------------------------------------------------------------------------
void StringControl::Confirm()
{
	String data = Field->text().toLatin1().data();
	if (data == *reinterpret_cast<String*>(Object) || DisableEdit)
		return;

	ControlCommand* cmd = new ControlCommand();
	cmd->Object = Object;
	cmd->Control = this;
	cmd->UndoValue = new String(*reinterpret_cast<String*>(Object));

	*reinterpret_cast<String*>(Object) = Field->text().toLatin1().data();

	cmd->RedoValue = new String(*reinterpret_cast<String*>(Object));

	cmd->UndoPtr = [](ControlCommand* c)
	{
		*reinterpret_cast<String*>(c->Object) = *reinterpret_cast<String*>(c->UndoValue);
		emit c->Control->ObjectUpdated(c);
	};
	cmd->RedoPtr = [](ControlCommand* c)
	{
		*reinterpret_cast<String*>(c->Object) = *reinterpret_cast<String*>(c->RedoValue);
		emit c->Control->ObjectUpdated(c);
	};

	emit ObjectUpdated(cmd);
}
