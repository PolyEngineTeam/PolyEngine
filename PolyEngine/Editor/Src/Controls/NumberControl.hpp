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
};

ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::INT8)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::INT16)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::INT32)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::INT64)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::UINT8)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::UINT16)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::UINT32)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::UINT64)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::FLOAT)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::DOUBLE)