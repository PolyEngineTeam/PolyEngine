#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtCore/qstring.h>

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;

class EntityDialog : public QDialog
{
public:
	EntityDialog() {}

	Dynarray<Entity*> SpawnEntities(World* world, Dynarray<Entity*> parents);
	void DestroyEntities(Dynarray<Entity*> parents);
	Entity* ReparentEntities(Dynarray<Entity*> entities, Entity* parent = nullptr);

	bool OperationCanceled() { return Canceled; }

private:
	void AddEntity(Entity* entity);
	void AddPrefab(QString prefabName);

	bool Canceled = true;

	QGridLayout* MainLayout;
	QTreeWidget* EntitiesTree;
	QTreeWidget* PrefabTree;
	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};