#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qgridlayout.h>

class StringControl : public ControlBase
{
public:
	StringControl(QWidget* parent);

	void Reset() override;
	void UpdateObject() override;
	void UpdateControl() override;
	void Confirm() override;

	void SetText(String text) { Field->setText(text.GetCStr()); UpdateObject(); }

private:
	QGridLayout* Layout;
	QLineEdit* Field;
};