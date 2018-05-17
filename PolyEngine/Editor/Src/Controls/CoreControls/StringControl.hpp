#pragma once

#include "Controls/ControlBase.hpp"

#include <QLineEdit.h>
#include <QGridLayout.h>

class StringControl : public ControlBase
{
public:
	StringControl(QWidget* parent);

	void UpdateObject() override;
	void UpdateControl() override;

private:
	QGridLayout* Layout;
	QLineEdit* Field;
};