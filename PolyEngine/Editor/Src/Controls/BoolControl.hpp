#pragma once

#include "RTTIBaseControl.hpp"

#include <qpushbutton.h>
#include <qstatemachine.h>

class BoolControl : public RTTIBaseControl
{
public:
	BoolControl(void* ptr, bool initialState = false);
	BoolControl(void* ptr, const RTTI::Property& prop);

	void UpdateObject() override;
	void UpdateControl() override;

private:
	void InitializeButton(bool initialState);

	bool* Object;

	QPushButton button;
	QStateMachine machine;

	std::unique_ptr<QState> False;
	std::unique_ptr<QState> True;
};