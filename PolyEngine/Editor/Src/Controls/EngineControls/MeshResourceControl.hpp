#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qgridlayout.h>

class MeshResourceControl : public ControlBase<Poly::MeshResource*>
{
public:
	MeshResourceControl(QWidget* parent);

	void Reset() override;

	void UpdateControl() override;

	void UpdateObject();

private:
	QGridLayout* Layout;
	QLineEdit* Field;
	QPushButton* BrowseButton;
	QComboBox* ResourceSourceComboBox;
	QPushButton* ConfirmButon;

private slots:
	void BrowseButtonClicked();
};