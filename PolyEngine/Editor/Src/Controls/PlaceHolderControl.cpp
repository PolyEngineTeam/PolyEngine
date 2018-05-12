#include "PlaceHolderControl.hpp"

void PlaceHolderControl::InitializeControl()
{
	Layout = std::make_unique<QGridLayout>();
	setLayout(Layout.get());

	QPalette disabledEditPalette;
	disabledEditPalette.setColor(QPalette::Base, QColor(218, 218, 218));
	disabledEditPalette.setColor(QPalette::Text, Qt::black);

	Field = std::make_unique<QLineEdit>(this);
	Field->setText("this is placeholder control for some type...");
	Field->setReadOnly(true);
	Field->setPalette(disabledEditPalette);

	Layout->addWidget(Field.get());
}
