#pragma once

#include "RTTIBaseControl.hpp"

#include <qpushbutton.h>
#include <qstatemachine.h>

class BoolControl : public RTTIBaseControl
{
public:
	BoolControl(void* ptr, const RTTI::Property& prop, RTTI::eCorePropertyType);

	void UpdateObject() override;
	void UpdateControl() override;

private:
	void InitializeControl();

	bool* Object;

	std::unique_ptr<QPushButton> Button;
	std::unique_ptr<QStateMachine> Machine;

	std::unique_ptr<QState> False;
	std::unique_ptr<QState> True;
};

ASSIGN_CONTROL(BoolControl, RTTI::eCorePropertyType::BOOL)