#include "PolyEditorPCH.hpp"

ASSIGN_CONTROL(BoolControl, BOOL)

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
	setToolTip("BOOL");
}

BoolControl::~BoolControl()
{
	delete True;
	delete False;
}

void BoolControl::UpdateObject()
{
	*reinterpret_cast<bool*>(Object) = *Machine->configuration().begin() == True;
}

void BoolControl::UpdateControl()
{
	Machine->setInitialState(*reinterpret_cast<bool*>(Object) ? True : False);
}
