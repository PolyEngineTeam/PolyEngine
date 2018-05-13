#pragma once

#include "ControlBase.hpp"

#include <qpushbutton.h>
#include <qstatemachine.h>

class BoolControl : public ControlBase
{
public:
	BoolControl() = default;
	~BoolControl();

	void UpdateObject() override;
	void UpdateControl() override;

private:
	void InitializeControl() override;

	QPushButton* Button;
	QStateMachine* Machine;
	QGridLayout* Layout;

	QState* False;
	QState* True;
};