#include "PolyEditorPCH.hpp"

BoolControl::BoolControl(QWidget* parent)
	: ControlBase(parent)
{
}

void BoolControl::UpdateObject()
{
	*reinterpret_cast<bool*>(Object) = *Machine->configuration().begin() == True.get();
}

void BoolControl::UpdateControl()
{
	Machine->stop();
	Machine->setInitialState(*reinterpret_cast<bool*>(Object) ? True.get() : False.get());
	Machine->start();
}

void BoolControl::InitializeControl()
{
	Button = std::make_unique<QPushButton>(this);
	Machine = std::make_unique<QStateMachine>(this);

	False = std::make_unique<QState>();
	False->assignProperty(Button.get(), "text", "False");
	False->setObjectName("false");

	True = std::make_unique<QState>();
	True->setObjectName("true");
	True->assignProperty(Button.get(), "text", "True");

	False->addTransition(Button.get(), SIGNAL(clicked()), True.get());
	True->addTransition(Button.get(), SIGNAL(clicked()), False.get());

	Machine->addState(False.get());
	Machine->addState(True.get());

	Machine->setInitialState(*reinterpret_cast<bool*>(Object) ? True.get() : False.get());
	Machine->start();
}
