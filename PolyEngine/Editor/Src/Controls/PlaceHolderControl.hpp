#pragma once

#include "ControlBase.hpp"

#include <QLineEdit.h>
#include <QGridLayout.h>

class PlaceHolderControl : public ControlBase
{
public:
	PlaceHolderControl();

	void UpdateObject() override {};
	void UpdateControl() override {};

private:
	QLineEdit* Field;
	QGridLayout* Layout;
};