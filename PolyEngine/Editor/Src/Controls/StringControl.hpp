#pragma once

#include "ControlBase.hpp"

#include <QLineEdit.h>

class StringControl : public ControlBase
{
public:
	StringControl(QWidget* parent);

	void UpdateObject() override;
	void UpdateControl() override;

private:
	void InitializeControl() override;

	std::unique_ptr<QLineEdit> Field;
};