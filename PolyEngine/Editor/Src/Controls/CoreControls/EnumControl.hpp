#pragma once

#include "Controls/ControlBase.hpp"

using namespace Poly;

class EnumControl : public ControlBase
{
public:
	EnumControl(QWidget* parent) : ControlBase(parent) {}

	void Reset() override;

	void UpdateObject() override;
	void UpdateControl() override;

protected:
	void Confirm() override;

private:
	//QComboBox* ComboBox; fix for clang compiler (unused private field)
};