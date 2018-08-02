#pragma once

#include "Controls/IControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>

class Vector2Control : public IControlBase
{
public:
	Vector2Control(QWidget* parent);

	void Reset() override;
	void UpdateObject() override;
	void UpdateControl() override;
	void Confirm() override;

private:
	QGridLayout* Layout;
	QLineEdit* Field[2];
};