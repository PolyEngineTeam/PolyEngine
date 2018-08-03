#include "PlaceHolderControl.hpp"

PlaceHolderControl::PlaceHolderControl(QWidget* parent)
	: ControlBase(parent)
{
	QPalette disabledEditPalette;
	disabledEditPalette.setColor(QPalette::Base, QColor(218, 218, 218));
	disabledEditPalette.setColor(QPalette::Text, Qt::black);

	Field = new QLineEdit(this);
	Field->setText("this is placeholder control for some type...");
	Field->setReadOnly(true);
	Field->setPalette(disabledEditPalette);

	Layout->addWidget(Field, 0, 1);
}

void PlaceHolderControl::Reset()
{
}
