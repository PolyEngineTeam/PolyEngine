#pragma once

#include <QtWidgets/qlineedit.h>

#include "Controls/ControlBase.hpp"

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
};