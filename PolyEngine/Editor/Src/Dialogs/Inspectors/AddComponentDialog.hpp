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

class AddComponentDialog : public QDialog
{
public:
	AddComponentDialog(Entity* entity);

	bool OperationCanceled() { return Canceled; }

private:
	Entity* Entity;
	bool Canceled = false;

	typedef ComponentBase* (*ComponentCreator)();
	std::map<QString, ComponentCreator> ComponentCreators;

	QGridLayout* MainLayout;

	QTreeWidget* Tree;

	QLabel* EntityIdNameText;		QLineEdit* EntityIdNameField;
	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};