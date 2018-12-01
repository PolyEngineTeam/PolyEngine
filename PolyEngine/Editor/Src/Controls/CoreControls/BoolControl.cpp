#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

ASSIGN_CONTROL(BoolControl, RTTI::eCorePropertyType::BOOL, BOOL)

//------------------------------------------------------------------------------
BoolControl::BoolControl(QWidget* parent)
	: ControlBase(parent)
{
	Button = new QPushButton(this);
	connect(Button, &QPushButton::clicked, this, &BoolControl::UpdateObject);
	Layout->addWidget(Button, 0, 1);

	Machine = new QStateMachine(this);

	False = new QState();
	False->setObjectName("false");
	False->assignProperty(Button, "text", "False");

	True = new QState();
	True->setObjectName("true");
	True->assignProperty(Button, "text", "True");

	False->addTransition(Button, SIGNAL(clicked()), True);
	True->addTransition(Button, SIGNAL(clicked()), False);

	Machine->addState(False);
	Machine->addState(True);

	Machine->setInitialState(False);
	Machine->start();
}

//------------------------------------------------------------------------------
void BoolControl::UpdateControl()
{
	if (Button->hasFocus())
		return;

	if ((*Machine->configuration().begin() == True) != *Object)
	{
		auto tmp = DisableEdit;
		DisableEdit = false;
		InitializationConfirm = true;
		emit Button->clicked();
		InitializationConfirm = false;
		DisableEdit = tmp;
	}
}

//------------------------------------------------------------------------------
void BoolControl::UpdateObject()
{
	if (DisableEdit || InitializationConfirm || *Object == (*Machine->configuration().begin() == True))
		return;

	ControlCommand<bool>* cmd = new ControlCommand<bool>();
	cmd->Object = Object;
	cmd->Control = this;
	cmd->UndoValue = new bool(*Object);
	cmd->RedoValue = new bool(*Machine->configuration().begin() == True);

	*Object = *Machine->configuration().begin() == True;

	emit ObjectUpdated(cmd);
}
