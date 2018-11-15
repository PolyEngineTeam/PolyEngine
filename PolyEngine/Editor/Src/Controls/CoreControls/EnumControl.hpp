#pragma once

#include "Controls/ControlBase.hpp"

using namespace Poly;

class EnumControl : public ControlBase<void>
{
public:
	EnumControl(QWidget* parent) : ControlBase(parent) {}

	void Reset() override;

	void UpdateControl() override;

protected:
	void UpdateObject();

private:
	//QComboBox* ComboBox; fix for clang compiler (unused private field)
};