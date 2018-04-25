#pragma once

#include "RTTIBaseControl.hpp"

using namespace Poly;

class EnumControl : public RTTIBaseControl
{
public:
	EnumControl(void* ptr, const RTTI::Property& prop, RTTI::eCorePropertyType);

	void UpdateObject() override;
	void UpdateControl() override;

	QWidget* GetField() override { return nullptr; }

private:
	std::unique_ptr<QComboBox> ComboBox;
	void* Object;
};

ASSIGN_CONTROL(EnumControl, RTTI::eCorePropertyType::ENUM)