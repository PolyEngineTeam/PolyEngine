#pragma once

#include "Controls/ControlBase.hpp"

using namespace Poly;

class EnumControl : public IControlBase
{
public:
	EnumControl(QWidget* parent) : IControlBase(parent) {}

	void Reset() override;

	void UpdateObject() override;
	void UpdateControl() override;

protected:
	void Confirm() override;

private:
	//QComboBox* ComboBox; fix for clang compiler (unused private field)
};