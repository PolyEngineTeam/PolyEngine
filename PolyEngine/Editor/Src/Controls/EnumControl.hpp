#pragma once

#include "ControlBase.hpp"

using namespace Poly;

class EnumControl : public ControlBase
{
public:
	EnumControl(QWidget* parent);

	void UpdateObject() override;
	void UpdateControl() override;

private:
	void InitializeControl() override;

	std::unique_ptr<QComboBox> ComboBox;
};

ASSIGN_CONTROL(EnumControl, RTTI::eCorePropertyType::ENUM)