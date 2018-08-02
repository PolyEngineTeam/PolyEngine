#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qgridlayout.h>

class MeshResourceControl : public ControlBase
{
public:
	MeshResourceControl(QWidget* parent);

	void Reset() override;

	void UpdateControl() override;

	void Confirm();

private:
	QGridLayout* Layout;
	QLineEdit* Field;
	QPushButton* BrowseButton;
	QComboBox* ResourceSourceComboBox;
	QPushButton* ConfirmButon;

private slots:
	void BrowseButtonClicked();
};