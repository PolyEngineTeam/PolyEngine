#pragma once

#include "Controls/IControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qgridlayout.h>

class MeshResourceControl : public IControlBase
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