#pragma once

#include "ControlBase.hpp"

#include <QLineEdit.h>
#include <QGridLayout.h>

class PlaceHolderControl : public ControlBase
{
public:
	PlaceHolderControl() = default;

	void UpdateObject() override {};
	void UpdateControl() override {};

private:
	void InitializeControl() override;

	std::unique_ptr<QLineEdit> Field;
	std::unique_ptr<QGridLayout> Layout;
};