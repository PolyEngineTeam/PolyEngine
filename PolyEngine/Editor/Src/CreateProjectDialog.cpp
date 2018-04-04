#include "PolyEditorPCH.hpp"

#include <QFileDialog>

CreateProjectDialog::CreateProjectDialog()
{
	setSizeGripEnabled(false);
	setModal(true);
	

	setWindowTitle("Project Creator");
	setFixedSize(600, 156);

	// ok cancel
	OkButton = new QPushButton(this);
	OkButton->move(513, 124);
	OkButton->resize(75, 20);
	OkButton->setText("OK");
	QObject::connect(OkButton, &QPushButton::clicked, this, &CreateProjectDialog::Ok);

	CancelButton = new QPushButton(this);
	CancelButton->move(430, 124);
	CancelButton->resize(75, 20);
	CancelButton->setText("Cancel");
	QObject::connect(CancelButton, &QPushButton::clicked, this, &CreateProjectDialog::Cancel);

	// project name
	ProjectNameText = new QLabel(this);
	ProjectNameText->move(12, 12);
	ProjectNameText->resize(100, 20);
	ProjectNameText->setText("Project Name");
	
	ProjectNameField = new QLineEdit(this);
	ProjectNameField->move(124, 12);
	ProjectNameField->resize(350, 20);
	ProjectNameField->setText("NewPolyPrject");

	// project directory
	ProjectDirectoryText = new QLabel(this);
	ProjectDirectoryText->move(12, 40);
	ProjectDirectoryText->resize(100, 20);
	ProjectDirectoryText->setText("Project Directory");

	ProjectDirectoryField = new QLineEdit(this);
	ProjectDirectoryField->move(124, 40);
	ProjectDirectoryField->resize(350, 20);

	BrowseDirectoryButton = new QPushButton(this);
	BrowseDirectoryButton->move(513, 40);
	BrowseDirectoryButton->resize(75, 20);
	BrowseDirectoryButton->setText("Browse");
	QObject::connect(BrowseDirectoryButton, &QPushButton::clicked, this, &CreateProjectDialog::BrowseProjectDirectory);

	// engine directory
	EngineDirectoryText = new QLabel(this);
	EngineDirectoryText->move(12, 68);
	EngineDirectoryText->resize(100, 20);
	EngineDirectoryText->setText("Engine Directory");

	EngineDirectoryField = new QLineEdit(this);
	EngineDirectoryField->move(124, 68);
	EngineDirectoryField->resize(350, 20);
	EngineDirectoryField->setText("..");

	EngineDirectoryButton = new QPushButton(this);
	EngineDirectoryButton->move(513, 68);
	EngineDirectoryButton->resize(75, 20);
	EngineDirectoryButton->setText("Browse");
	QObject::connect(EngineDirectoryButton, &QPushButton::clicked, this, &CreateProjectDialog::BrowseEngineDirectory);
}

CreateProjectDialog::~CreateProjectDialog()
{
	delete ProjectNameText;
	delete ProjectNameField;

	delete ProjectDirectoryText;
	delete ProjectDirectoryField;
	delete BrowseDirectoryButton;

	delete EngineDirectoryText;
	delete EngineDirectoryField;
	delete EngineDirectoryButton;

	delete OkButton;
	delete CancelButton;
}

void CreateProjectDialog::BrowseProjectDirectory()
{
	QFileDialog fileDialog;
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setFileMode(QFileDialog::Directory);
	if (fileDialog.exec() == QDialog::Accepted)
		ProjectDirectoryField->setText(fileDialog.selectedFiles()[0]);
}

void CreateProjectDialog::BrowseEngineDirectory()
{
	QFileDialog fileDialog;
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setFileMode(QFileDialog::Directory);
	if (fileDialog.exec() == QDialog::Accepted)
		EngineDirectoryField->setText(fileDialog.selectedFiles()[0]);
}

void CreateProjectDialog::Ok()
{
	close();
}

void CreateProjectDialog::Cancel()
{
	Canceled = true;

	close();
}
