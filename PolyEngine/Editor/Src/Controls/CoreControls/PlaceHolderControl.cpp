#include "PlaceHolderControl.hpp"

PlaceHolderControl::PlaceHolderControl(QWidget* parent)
	: IControlBase(parent)
{
	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	QPalette disabledEditPalette;
	disabledEditPalette.setColor(QPalette::Base, QColor(218, 218, 218));
	disabledEditPalette.setColor(QPalette::Text, Qt::black);

	Field = new QLineEdit(this);
	Field->setText("this is placeholder control for some type...");
	Field->setReadOnly(true);
	Field->setPalette(disabledEditPalette);

	Layout->addWidget(Field);

	setLayout(Layout);
}

void PlaceHolderControl::Reset()
{
}
