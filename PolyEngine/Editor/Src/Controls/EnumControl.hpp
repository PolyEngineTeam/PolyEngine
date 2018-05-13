#pragma once

#include "ControlBase.hpp"

using namespace Poly;

class EnumControl : public ControlBase
{
public:
	EnumControl() = default;;

	void UpdateObject() override;
	void UpdateControl() override;

private:
	void InitializeControl() override;

	QComboBox* ComboBox;
};