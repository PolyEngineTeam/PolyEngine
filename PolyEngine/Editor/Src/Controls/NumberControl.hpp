#pragma once

#include "ControlBase.hpp"

#include <QLineEdit.h>

class NumberControl : public ControlBase
{
public:
	NumberControl() = default;

	void UpdateObject() override;
	void UpdateControl() override;

private:
	void InitializeControl() override;

	QLineEdit* Field;
	QValidator* Validator;
	QGridLayout* Layout;
};