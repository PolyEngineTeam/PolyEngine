#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qgridlayout.h>

class StringControl : public ControlBase<Poly::String>
{
public:
	StringControl(QWidget* parent);

	void Reset() override;
	void UpdateControl() override;

	void SetText(String text) { Field->setText(text.GetCStr()); }

private:
	void UpdateObject();

	QGridLayout* Layout;
	QLineEdit* Field;
};