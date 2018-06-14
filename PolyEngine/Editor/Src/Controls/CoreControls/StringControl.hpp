#pragma once

#include "Controls/ControlBase.hpp"

#include <QLineEdit.h>
#include <QGridLayout.h>

class StringControl : public ControlBase
{
public:
	StringControl(QWidget* parent);

	void Reset() override;

	void UpdateObject() override;
	void UpdateControl() override;

	void SetText(String text) { Reset(); Field->setText(text.GetCStr); }

private:
	QGridLayout* Layout;
	QLineEdit* Field;
};