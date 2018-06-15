#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QtWidgets/qlabel.h>
#include <QPushButton>

class CreateProjectDialog : public QDialog
{
public:
	CreateProjectDialog();
	~CreateProjectDialog();

	bool OperationCanceled() { return Canceled; }
	QString GetProjectName() { return ProjectNameField->text(); }
	QString GetProjectDirectory() { return ProjectDirectoryField->text(); }
	QString GetEngineDirectory() { return EngineDirectoryField->text(); }

private:
	QLabel* ProjectNameText;
	QLineEdit* ProjectNameField;

	QLabel* ProjectDirectoryText;
	QLineEdit* ProjectDirectoryField;
	QPushButton* BrowseDirectoryButton;

	QLabel* EngineDirectoryText;
	QLineEdit* EngineDirectoryField;
	QPushButton* EngineDirectoryButton;

	QPushButton* OkButton;
	QPushButton* CancelButton;

	bool Canceled = false;

private slots:
	void BrowseProjectDirectory();
	void BrowseEngineDirectory();
	void Ok();
	void Cancel();
};
