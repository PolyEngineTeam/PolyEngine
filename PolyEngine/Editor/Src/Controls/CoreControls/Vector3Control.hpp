#pragma once

#include "Controls/IControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>

class Vector3Control : public IControlBase
{
public:
	Vector3Control(QWidget* parent);

	void Reset() override;
	void UpdateObject() override;
	void UpdateControl() override;
	void Confirm() override;

private:
	QGridLayout* Layout;
	QLineEdit* Field[3];
};