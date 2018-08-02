#include "PolyEditorPCH.hpp"

#include <Resources/ResourceManager.hpp>
#include <Resources/MeshResource.hpp>
#include <QtWidgets/QFileDialog>

//------------------------------------------------------------------------------
MeshResourceControl::MeshResourceControl(QWidget* parent)
	: ControlBase(parent)
{ 
	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Layout->setColumnStretch(0, 20);
	Layout->setColumnStretch(1, 1);
	Layout->setColumnStretch(2, 1);
	Layout->setColumnStretch(3, 5);
	Layout->setColumnStretch(4, 5);
	Layout->setColumnStretch(5, 5);

	Field = new QLineEdit(this);
	Layout->addWidget(Field, 0, 0);

	BrowseButton = new QPushButton(this);
	BrowseButton->setText("...");
	BrowseButton->setMaximumWidth(30);
	connect(BrowseButton, &QPushButton::pressed, this, &MeshResourceControl::BrowseButtonClicked);
	Layout->addWidget(BrowseButton, 0, 1);

	ResourceSourceComboBox = new QComboBox(this);
	ResourceSourceComboBox->addItem("Runtime Dir.");
	ResourceSourceComboBox->addItem("Engine");
	ResourceSourceComboBox->addItem("Game");
	ResourceSourceComboBox->addItem("User Local");
	ResourceSourceComboBox->setMaximumWidth(80);
	Layout->addWidget(ResourceSourceComboBox, 0, 3);

	ConfirmButon = new QPushButton(this);
	ConfirmButon->setText("Reload Resource");
	ConfirmButon->setMaximumWidth(100);
	connect(ConfirmButon, &QPushButton::pressed, this, &MeshResourceControl::UpdateObject);
	Layout->addWidget(ConfirmButon, 0, 5);

	setLayout(Layout);
}

//------------------------------------------------------------------------------
void MeshResourceControl::Reset()
{
	ControlBase::Reset();
	Field->setText("");
}

//------------------------------------------------------------------------------
void MeshResourceControl::UpdateControl()
{
}

//------------------------------------------------------------------------------
void MeshResourceControl::UpdateObject()
{
	eResourceSource source;

	switch (ResourceSourceComboBox->currentIndex())
	{
	case 0:
		source = eResourceSource::NONE;
		break;

	case 1:
		source = eResourceSource::ENGINE;
		break;

	case 2:
		source = eResourceSource::GAME;
		break;

	case 3:
		source = eResourceSource::USR_LOCAL;
		break;
	}

	*Object = ResourceManager<MeshResource>::Load(Field->text().toLatin1().data(), source);
}

//------------------------------------------------------------------------------
void MeshResourceControl::BrowseButtonClicked()
{
	QString basePath;
	switch (ResourceSourceComboBox->currentIndex())
	{
	case 0:
		basePath = gAssetsPathConfig.GetAssetsPath(eResourceSource::NONE).GetCStr();
		break;

	case 1:
		basePath = gAssetsPathConfig.GetAssetsPath(eResourceSource::ENGINE).GetCStr();
		break;

	case 2:
		basePath = gAssetsPathConfig.GetAssetsPath(eResourceSource::GAME).GetCStr();
		break;

	case 3:
		basePath = gAssetsPathConfig.GetAssetsPath(eResourceSource::USR_LOCAL).GetCStr();
		break;
	}

	QFileDialog fileDialog;
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setDirectory(basePath);

	if (fileDialog.exec() == QDialog::Accepted)
	{
		Field->setText(((QString)fileDialog.selectedFiles()[0]).remove(0, basePath.length()));
		UpdateObject();
	}
}
