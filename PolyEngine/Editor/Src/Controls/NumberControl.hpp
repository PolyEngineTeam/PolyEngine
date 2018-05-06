#pragma once

#include "ControlBase.hpp"

#include <QLineEdit.h>

class NumberControl : public ControlBase
{
public:
	NumberControl(QWidget* parent);

	void UpdateObject() override;
	void UpdateControl() override;

private:
	void InitializeControl() override;

	std::unique_ptr<QLineEdit> Field;
	std::unique_ptr<QValidator> Validator;
	std::unique_ptr<QGridLayout> Layout;
};