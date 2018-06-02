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
	ComponentDialog(::World* world, bool removeComponents = false);

	bool OperationCanceled() { return Canceled; }

private:
	Entity* Entity;
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
