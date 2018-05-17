#pragma once

#include "Controls/ControlBase.hpp"

#include <QLineEdit.h>
#include <qlabel.h>
#include <qgridlayout.h>

class TransformControlGroup : public ControlBase
{
public:
	TransformControlGroup(QWidget* parent);

	void UpdateObject() override;
	void UpdateControl() override;

private:
	QGridLayout* Layout;
								QLabel * XYZLabels[3];
	QLabel* TranslationLabel;	QLineEdit* TranslationField[3];
	QLabel* RotationLabel;		QLineEdit* RotationField[3];
	QLabel* ScaleLabel;			QLineEdit* ScaleField[3];
};