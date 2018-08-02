#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlineedit.h>

class PlaceHolderControl : public ControlBase<void>
{
public:
	PlaceHolderControl(QWidget* parent);

	void Reset() override;

	void UpdateControl() override {};

protected:
	void UpdateObject() {};

private:
	QLineEdit* Field;
	QGridLayout* Layout;
};