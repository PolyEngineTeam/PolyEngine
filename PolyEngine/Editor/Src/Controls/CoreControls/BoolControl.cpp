#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

ASSIGN_CONTROL(BoolControl, RTTI::eCorePropertyType::BOOL, BOOL)

//------------------------------------------------------------------------------
BoolControl::BoolControl(QWidget* parent)
	: ControlBase(parent)
{
	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Button = new QPushButton(this);
	connect(Button, &QPushButton::clicked, this, &ControlBase::Confirm);
	Layout->addWidget(Button);

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

	setLayout(Layout);
}

//------------------------------------------------------------------------------
BoolControl::~BoolControl()
{
	delete True;
	delete False;
}

//------------------------------------------------------------------------------
void BoolControl::Reset()
{
	Object = nullptr;
}

//------------------------------------------------------------------------------
void BoolControl::UpdateObject()
{
	*reinterpret_cast<bool*>(Object) = *Machine->configuration().begin() == True;
}

//------------------------------------------------------------------------------
void BoolControl::UpdateControl()
{
	if (Button->hasFocus())
		return;

	auto control = *Machine->configuration().begin() == True;
	auto object = *reinterpret_cast<bool*>(Object);

	if ((*Machine->configuration().begin() == True) != *reinterpret_cast<bool*>(Object))
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
void BoolControl::Confirm()
{
	if (DisableEdit || InitializationConfirm)
		return;

	ControlCommand<bool>* cmd = new ControlCommand<bool>();
	cmd->Object = Object;
	cmd->Control = this;
	cmd->UndoValue = new bool(*reinterpret_cast<bool*>(Object));
	cmd->RedoValue = new bool(*Machine->configuration().begin() == True);

	*reinterpret_cast<bool*>(Object) = *Machine->configuration().begin() == True;

	emit ObjectUpdated(cmd);
}