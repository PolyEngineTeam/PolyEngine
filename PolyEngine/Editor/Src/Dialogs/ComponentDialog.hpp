#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <qgroupbox.h>
#include <QPushButton>
#include <qcheckbox.h>

#include <ECS/Entity.hpp>

using namespace Poly;

class ComponentDialog : public QDialog
{
public:
	ComponentDialog(::Entity* entity, bool removeComponents = false);

	bool OperationCanceled() { return Canceled; }

private:
	Entity* Entity;

	bool Canceled = false;

	QScrollArea* ScrollArea;
	QGridLayout* MainLayout;

	QGroupBox* GroupBox;
	QGridLayout* GroupBoxLayout;
	Dynarray<QCheckBox*> CheckBoxes;
	QLabel* EntityIdNameText;		QLineEdit* EntityIdNameField;
	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void BrowseProjectDirectory();
	void BrowseEngineDirectory();
	void Ok();
	void Cancel();
};
