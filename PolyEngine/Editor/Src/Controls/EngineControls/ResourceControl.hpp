#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qgridlayout.h>

class ResourceControl : public ControlBase
{
public:
	ResourceControl(QWidget* parent);

	void Reset() override;

	void UpdateObject() override;
	void UpdateControl() override;

	void Confirm() override;

private:
	void ChangeResourceDialog();

	eResourceSource Location;

	QGridLayout* Layout;
	QLineEdit* Field;
	QPushButton* Button;
};