#pragma once

#include "Controls/ControlBase.hpp"

#include <qpushbutton.h>
#include <qstatemachine.h>

class BoolControl : public ControlBase
{
public:
	BoolControl(QWidget* parent);
	~BoolControl();

	void Reset() override;

	void UpdateObject() override;
	void UpdateControl() override;

private:
	QPushButton* Button;
	QStateMachine* Machine;
	QGridLayout* Layout;

	QState* False;
	QState* True;
};