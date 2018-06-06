#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <qgroupbox.h>
#include <QPushButton>
#include <qcheckbox.h>
#include <qtreewidget.h>

#include <ECS/World.hpp>

using namespace Poly;

class WorldComponentDialog : public QDialog
{
public:
	WorldComponentDialog(::World* world, bool removeComponents = false);

	bool OperationCanceled() { return Canceled; }

private:
	World* World;

	bool Canceled = false;

	QGridLayout* MainLayout;

	QTreeWidget* Tree;

	QLabel* EntityIdNameText;		QLineEdit* EntityIdNameField;
	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};
