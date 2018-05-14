#pragma once

#include "ControlBase.hpp"

#include <QLineEdit.h>

class NumberControl : public ControlBase
{
public:
	NumberControl();
	~NumberControl();

	void UpdateObject() override;
	void UpdateControl() override;

private:
	QLineEdit* Field;
	QValidator* Validator;
	QGridLayout* Layout;
};