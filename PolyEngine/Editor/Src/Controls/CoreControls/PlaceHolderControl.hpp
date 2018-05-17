#pragma once

#include "Controls/ControlBase.hpp"

#include <QLineEdit.h>
#include <QGridLayout.h>

class PlaceHolderControl : public ControlBase
{
public:
	PlaceHolderControl(QWidget* parent);

	void UpdateObject() override {};
	void UpdateControl() override {};

private:
	QLineEdit* Field;
	QGridLayout* Layout;
};