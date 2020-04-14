#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtCore/qstring.h>

#include <pe/api/ecs/Scene.hpp>
#include <pe/api/ecs/Entity.hpp>

using namespace Poly;

class EntityDialog : public QDialog
{
public:
	EntityDialog() {}

	std::vector<Entity*> SpawnEntities(Scene* scene, std::vector<Entity*> parents);
	std::vector<Entity*> GetEntitiesToDestroy(Scene* scene, std::vector<Entity*> entities);
	std::vector<Entity*> ReparentEntities(Scene* scene, std::vector<Entity*> entities, Entity* parent = nullptr);

	bool OperationCanceled() { return Canceled; }

private:
	void AddEntity(Entity* entity);
	void AddPrefab(QString prefabName);

	bool Canceled = true;
	std::vector<Entity*> PredefinedEntities;
	std::map<QTreeWidgetItem*, Entity*> ItemToEntity;

	QGridLayout* MainLayout;
	QLabel* FirstLabel;
	QTreeWidget* EntitiesTree;
	QLabel* SecondLabel;
	QTreeWidget* PrefabTree;
	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};