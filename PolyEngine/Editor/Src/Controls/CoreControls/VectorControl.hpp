#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>

class VectorControl : public ControlBase
{
public:
	VectorControl(QWidget* parent);

	void Reset() override;

	void UpdateObject() override;
	void UpdateControl() override;

private:
	QGridLayout* Layout;
	QLineEdit* Field[3];

	public slots:
	void Confirm() 
	{
		if (ASAPUpdate)
			QTimer::singleShot(1, this, [object = this]() { object->UpdateObject(); });
	}
};