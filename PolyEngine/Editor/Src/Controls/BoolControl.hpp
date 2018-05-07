#pragma once

#include "ControlBase.hpp"

#include <qpushbutton.h>
#include <qstatemachine.h>

class BoolControl : public ControlBase
{
public:
	BoolControl() = default;

	void UpdateObject() override;
	void UpdateControl() override;

private:
	void InitializeControl() override;

	std::unique_ptr<QPushButton> Button;
	std::unique_ptr<QStateMachine> Machine;
	std::unique_ptr<QGridLayout> Layout;

	std::unique_ptr<QState> False;
	std::unique_ptr<QState> True;
};