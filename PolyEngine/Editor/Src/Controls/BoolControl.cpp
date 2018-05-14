#include "PolyEditorPCH.hpp"

ASSIGN_CONTROL(BoolControl, BOOL)

BoolControl::BoolControl()
{
	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Button = new QPushButton(this);
	Machine = new QStateMachine(this);

	Layout->addWidget(Button);

	False = new QState();
	False->setObjectName("false");
	False->assignProperty(Button, "text", "False");

	True = new QState();
	True->setObjectName("true");
	True->assignProperty(Button, "text", "True");

	False->addTransition(Button, SIGNAL(clicked()), True);
	True->addTransition(Button, SIGNAL(clicked()), False);

	connect(Button, &QPushButton::clicked, this, &BoolControl::Clicked);

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
