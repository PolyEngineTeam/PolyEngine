#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtCore/qstring.h>
#include <QtWidgets/qtreewidget.h>

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;

class ResourceDialog : public QDialog
{
public:
	ResourceDialog() {}

	std::unique_ptr<MeshResource> SelectResource();

	bool OperationCanceled() { return Canceled; }

private:
	struct 

	void AddResource(Entity* entity);

	bool Canceled = true;
	Dynarray<Entity*> PredefinedEntities;
	std::map<QTreeWidgetItem*, String> ItemToEntity;

	QGridLayout* MainLayout;

	QTreeWidget* ResourcesTree;

	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};