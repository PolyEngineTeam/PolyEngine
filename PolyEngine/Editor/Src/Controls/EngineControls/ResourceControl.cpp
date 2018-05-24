#include "PolyEditorPCH.hpp"

ResourceControl::ResourceControl(QWidget* parent)
	: ControlBase(parent)
{
	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Field = new QLineEdit();
	connect(Field, &QLineEdit::editingFinished, this, &ControlBase::Confirm);

	Layout->addWidget(Field);

	setLayout(Layout);
}

void ResourceControl::UpdateObject()
{
	//*reinterpret_cast<String*>(Object) = Field->text().toLatin1().data();
}

void ResourceControl::UpdateControl()
{
	//Field->setText(reinterpret_cast<String*>(Object)->GetCStr());
}
