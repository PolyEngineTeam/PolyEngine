#pragma once

#include "RTTIBaseControl.hpp"

#include <QLineEdit.h>

class StringControl : public RTTIBaseControl
{
public:
	StringControl(void* ptr, const RTTI::Property& prop, RTTI::eCorePropertyType type);

	void UpdateObject() override;
	void UpdateControl() override;

private:
	void InitializeControl();

	String* Object;

	std::unique_ptr<QLineEdit> Field;
};

ASSIGN_CONTROL(StringControl, RTTI::eCorePropertyType::STRING)