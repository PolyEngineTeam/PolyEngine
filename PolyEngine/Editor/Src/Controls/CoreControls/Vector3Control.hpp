#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>

class Vector3Control : public ControlBase<Poly::Vector>
{
public:
	Vector3Control(QWidget* parent);

	void Reset() override;
	void UpdateControl() override;

private:
	void UpdateObject();

	QLineEdit* Field[3];
};