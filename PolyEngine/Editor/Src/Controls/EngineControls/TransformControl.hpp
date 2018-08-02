#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>

class TransformControl : public ControlBase<Poly::EntityTransform>
{
public:
	TransformControl(QWidget* parent);

	void Reset() override;

	void UpdateControl() override;

	bool ContainsLabel() override { return true; }

	void UpdateObject() {};

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
		QTimer::singleShot(1, this, [object = this]() { object->UodateTranslation(); });
	}

	void ConfirmRotation()
	{
		QTimer::singleShot(1, this, [object = this]() { object->UodateRotation(); });
	}

	void ConfirmScale()
	{
		QTimer::singleShot(1, this, [object = this]() { object->UodateScale(); });
	}
};