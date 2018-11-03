#pragma once

#include <QtWidgets/qlineedit.h>

#include "Controls/ControlBase.hpp"

class StringControl : public ControlBase<Poly::String>
{
public:
	StringControl(QWidget* parent);

	void Reset() override;
	void UpdateControl() override;

	void SetText(String text) { Field->setText(text.GetCStr()); }

private:
	void UpdateObject();

	QLineEdit* Field;
};