#pragma once

#include "Controls/IControlBase.hpp"

#include <QtWidgets/qlineedit.h>

class NumberControl : public IControlBase
{
public:
	NumberControl(QWidget* parent);

	void Reset() override;
	void UpdateObject() override;
	void UpdateControl() override;
	void Confirm() override;

private:
	QLineEdit* Field;
	QGridLayout* Layout;

	QRegExpValidator* UIntValidator;
	QRegExpValidator* IntValidator;
	QRegExpValidator* FloatValidator;
};