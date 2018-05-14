#pragma once

#include "ControlBase.hpp"

#include <QLineEdit.h>
#include <QGridLayout.h>

class StringControl : public ControlBase
{
public:
	StringControl();

	void UpdateObject() override;
	void UpdateControl() override;

private:
	QGridLayout* Layout;
	QLineEdit* Field;
};