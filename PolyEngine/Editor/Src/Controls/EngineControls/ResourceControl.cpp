#include "PolyEditorPCH.hpp"

#include <Resources/ResourceManager.hpp>
#include <Resources/MeshResource.hpp>

//------------------------------------------------------------------------------
ResourceControl::ResourceControl(QWidget* parent)
	: ControlBase(parent)
{
	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Field = new QLineEdit(this);
	Field->setDisabled(true);

	Button = new QPushButton(this);
	Button->setText("Change");
	connect(Button, &QPushButton::pressed, this, &ResourceControl::ChangeResourceDialog);

	Layout->addWidget(Field);

	setLayout(Layout);
}

//------------------------------------------------------------------------------
void ResourceControl::Reset()
{
	ControlBase::Reset();
	Field->setText("");
}

//------------------------------------------------------------------------------
void ResourceControl::UpdateObject()
{
	//String path = Field->text().toLatin1().data();
	//MeshResource* newMesh = ResourceManager<MeshResource>::Load(path, source);

	//*reinterpret_cast<String*>(Object) = Field->text().toLatin1().data();
}

//------------------------------------------------------------------------------
void ResourceControl::UpdateControl()
{
	//Field->setText(reinterpret_cast<String*>(Object)->GetCStr());
}

//------------------------------------------------------------------------------
void ResourceControl::Confirm()
{
}

//------------------------------------------------------------------------------
void ResourceControl::ChangeResourceDialog()
{
}
