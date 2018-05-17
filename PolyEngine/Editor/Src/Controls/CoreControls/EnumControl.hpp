#pragma once

#include "Controls/ControlBase.hpp"

using namespace Poly;

class EnumControl : public ControlBase
{
public:
	EnumControl(QWidget* parent) : ControlBase(parent) {}

	void UpdateObject() override;
	void UpdateControl() override;

private:
	QComboBox* ComboBox;
};