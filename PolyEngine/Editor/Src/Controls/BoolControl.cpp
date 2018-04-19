#include "BoolControl.hpp"

BoolControl::BoolControl(void* ptr, bool initialState = false)
{
	Object = reinterpret_cast<bool*>(ptr);

	InitializeButton(initialState);
}

BoolControl::BoolControl(void* ptr, const RTTI::Property & prop)
{
	Object = reinterpret_cast<bool*>(ptr);

	InitializeButton(*Object);
}

void BoolControl::InitializeButton(bool initialState)
{
	False = std::make_unique<QState>();
	False->assignProperty(&button, "text", "False");
	False->setObjectName("false");

	True = std::make_unique<QState>();
	True->setObjectName("true");
	True->assignProperty(&button, "text", "True");

	False->addTransition(&button, SIGNAL(clicked()), True.get());
	True->addTransition(&button, SIGNAL(clicked()), False.get());

	machine.addState(False.get());
	machine.addState(True.get());

	machine.setInitialState(initialState ? True.get() : False.get());
	machine.start();

	button.resize(100, 50);
	button.show();
}
