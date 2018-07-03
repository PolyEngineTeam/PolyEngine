#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlineedit.h>

class PlaceHolderControl : public ControlBase
{
public:
	PlaceHolderControl(QWidget* parent);

	void Reset() override;

	void UpdateObject() override {};
	void UpdateControl() override {};

protected:
	void Confirm() override {};

private:
	QLineEdit* Field;
	QGridLayout* Layout;
};