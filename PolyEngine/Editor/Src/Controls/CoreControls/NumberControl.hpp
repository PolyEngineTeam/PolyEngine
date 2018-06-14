#pragma once

#include "Controls/ControlBase.hpp"

#include <QLineEdit.h>

class NumberControl : public ControlBase
{
public:
	NumberControl(QWidget* parent);

	void Reset() override;

	void UpdateObject() override;
	void UpdateControl() override;

private:
	QLineEdit* Field;
	QGridLayout* Layout;
};