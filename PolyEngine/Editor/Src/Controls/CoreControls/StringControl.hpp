#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qgridlayout.h>

class StringControl : public ControlBase
{
public:
	StringControl(QWidget* parent);

	void Reset() override;
	void UpdateControl() override;
	void Confirm();

	void SetText(String text) { Field->setText(text.GetCStr()); }

private:
	QGridLayout* Layout;
	QLineEdit* Field;
};