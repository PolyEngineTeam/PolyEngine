#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qgridlayout.h>

class MeshResourceControl : public ControlBase
{
public:
	MeshResourceControl(QWidget* parent);

	void Reset() override;

	void UpdateObject() override;
	void UpdateControl() override;

	void Confirm() override;

private:
	QGridLayout* Layout;
	QLineEdit* Field;
	QPushButton* BrowseButton;
	QComboBox* ResourceSourceComboBox;
	QPushButton* ConfirmButon;

private slots:
	void BrowseButtonClicked();
};