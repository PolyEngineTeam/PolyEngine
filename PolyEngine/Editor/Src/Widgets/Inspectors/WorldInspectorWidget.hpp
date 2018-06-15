#pragma once

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>
#include <UniqueID.hpp>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtWidgets/qcombobox.h>
#include <QLayout>
#include <QLabel>

#include "Widgets/Inspectors/InspectorWidgetBase.hpp"

using namespace Poly;

class CustomTree : public QTreeWidget
{
	Q_OBJECT

public:
	CustomTree(QWidget* parent) : QTreeWidget(parent) {}

signals:
	void Dropped(QDropEvent* e);

private:
	void dropEvent(QDropEvent* e) override { emit Dropped(e);  QTreeWidget::dropEvent(e); }
};

// Displays tree of entities in the world. Enables adding, removing and reparenting entities.
class WorldInspectorWidget : public InspectorWidgetBase
{
	Q_OBJECT

public:
	WorldInspectorWidget(QWidget* parent);

	// Initializes object connections with other inspectors and inspector manager.
	void InitializeConnections() override;

	// Removes all entities from list.
	void Reset() override;

	// Updates names.
	void UpdateInspector() override;

	// Removes all entities from list and loads them again.
	void ReloadInspector() override;

	// Sets currently viewed world to given object and updates inspector.
	void SetObject(World* world);

	// Sets selected entities.
	void SetSelectedEntities(Dynarray<Entity*> entities);

private:
	void AddEntityToTree(Entity* entity, QTreeWidgetItem* parent);

	World* World;
	Dynarray<Entity*> SelectedEntities;

	QMenu* ContextMenu;
		QAction* AddEntityAction;
		QAction* RemoveEntityAction;
		QAction* ReparentEntityAction;

	CustomTree* Tree;
	std::map<QTreeWidgetItem*, Entity*> EntityFromItem;

private slots:
	void SelectionChanged(QTreeWidgetItem* item, int column);
	void EntitiesReparented(QDropEvent* e);

	void SpawnContextMenu(QPoint pos);
		void AddEntity();
		void RemoveEntity();
		void ReparentEntity();
};