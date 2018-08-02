#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qpushbutton.h>
#include <QtCore/qstatemachine.h>

class Command;

class BoolControl : public ControlBase
{
public:
	BoolControl(QWidget* parent);
	~BoolControl();

	void Reset() override;
	void UpdateControl() override;
	void Confirm();

private:
	QPushButton* Button;
	QStateMachine* Machine;
	QGridLayout* Layout;

	QState* False;
	QState* True;

	bool InitializationConfirm = false;
};