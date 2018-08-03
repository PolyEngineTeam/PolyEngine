#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qpushbutton.h>
#include <QtCore/qstatemachine.h>
#include <QtWidgets/qgridlayout.h>

class Command;

class BoolControl : public ControlBase<bool>
{
public:
	BoolControl(QWidget* parent);

	void UpdateControl() override;

private:
	void UpdateObject();

	QPushButton* Button;
	QStateMachine* Machine;
	QGridLayout* Layout;

	QState* False;
	QState* True;

	bool InitializationConfirm = false;
};