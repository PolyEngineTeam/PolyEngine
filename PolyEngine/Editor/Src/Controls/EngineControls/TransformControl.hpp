#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <qlabel.h>
#include <QtWidgets/qgridlayout.h>

class TransformControl : public ControlBase
{
public:
	TransformControl(QWidget* parent);

	void Reset() override;

	void UpdateObject() override;
	void UpdateControl() override;

	bool ContainsLabel() override { return true; }

private:
	void UodateTranslation();
	void UodateRotation();
	void UodateScale();

	QGridLayout* Layout;
								QLabel * XYZLabels[3];
	QLabel* TranslationLabel;	QLineEdit* TranslationField[3];
	QLabel* RotationLabel;		QLineEdit* RotationField[3];
	QLabel* ScaleLabel;			QLineEdit* ScaleField[3];

public slots:
	void ConfirmTranslation()
	{
		if (ASAPUpdate)
			QTimer::singleShot(1, this, [object = this]() { object->UodateTranslation(); });
	}

	void ConfirmRotation()
	{
		if (ASAPUpdate)
			QTimer::singleShot(1, this, [object = this]() { object->UodateRotation(); });
	}

	void ConfirmScale()
	{
		if (ASAPUpdate)
			QTimer::singleShot(1, this, [object = this]() { object->UodateScale(); });
	}
};