#pragma once

#include "ControlBase.hpp"

#include <qpushbutton.h>
#include <qstatemachine.h>

class BoolControl : public ControlBase
{
public:
	BoolControl();
	~BoolControl();

	void UpdateObject() override;
	void UpdateControl() override;

private:
	QPushButton* Button;
	QStateMachine* Machine;
	QGridLayout* Layout;

	QState* False;
	QState* True;
};