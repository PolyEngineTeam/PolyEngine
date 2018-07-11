#pragma once

#include "Controls/ControlBase.hpp"
#include "Math/Vector2f.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>

class Vector2Control : public ControlBase
{
public:
	Vector2Control(QWidget* parent);

	void Reset() override;

	void UpdateObject() override;
	void UpdateControl() override;

private:
	QGridLayout* Layout;
	QLineEdit* Field[2];

	public slots:
	void Confirm() 
	{
		if (ASAPUpdate)
			QTimer::singleShot(1, this, [object = this]() { object->UpdateObject(); });
	}
};