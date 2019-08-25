#pragma once

#include <QtWidgets/qlineedit.h>

#include "Controls/ControlBase.hpp"

class StringControl : public ControlBase<core::storage::String>
{
public:
	StringControl(QWidget* parent);

	void Reset() override;
	void UpdateControl() override;

	void SetText(core::storage::String text) { Field->setText(text.GetCStr()); }

private:
	void UpdateObject();

	QLineEdit* Field;
};