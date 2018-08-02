#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>

class NumberControl : public ControlBase
{
public:
	NumberControl(QWidget* parent);

	void Reset() override;
	void UpdateControl() override;
	void Confirm();

private:
	QLineEdit* Field;
	QGridLayout* Layout;

	QRegExpValidator* UIntValidator;
	QRegExpValidator* IntValidator;
	QRegExpValidator* FloatValidator;
};