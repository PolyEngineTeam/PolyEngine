#pragma once

#include <QtWidgets/qdialog.h>
#include <qlabel.h>
#include <QtWidgets/qgridlayout.h>
#include <qlistwidget.h>
#include <QtWidgets/qlineedit.h>

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;

class ChangeParentDialog : public QDialog
{
public:
	ChangeParentDialog(Dynarray<Entity*> entities, Entity* parent = nullptr);

	bool OperationCanceled() { return Canceled; }

private:
	void AddEntity(Entity* entity);

	Dynarray<Entity*> Entities;

	bool Canceled = false;

	QGridLayout* MainLayout;

	QTreeWidget* Tree;

	QLabel* EntityIdNameText;		QLineEdit* EntityIdNameField;
	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};