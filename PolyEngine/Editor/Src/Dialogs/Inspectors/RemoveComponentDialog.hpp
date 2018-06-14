#pragma once

#include <qdialog.h>
#include <qlabel.h>
#include <qgridlayout.h>
#include <qlistwidget.h>
#include <qlineedit.h>
#include <qtreewidget.h>
#include <qpushbutton.h>

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;

class RemoveComponentDialog : public QDialog
{
public:
	RemoveComponentDialog(Entity* entity);

	bool OperationCanceled() { return Canceled; }

private:
	Entity * Entity;
	bool Canceled = false;

	typedef void (*ComponentDestroyer)(::Entity* e);
	std::map<QString, ComponentDestroyer> ComponentDestroyers;

	QGridLayout* MainLayout;

	QTreeWidget* Tree;

	QLabel* EntityIdNameText;		QLineEdit* EntityIdNameField;
	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};