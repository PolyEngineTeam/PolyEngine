#pragma once

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>
#include <UniqueID.hpp>

#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qtreewidget.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtGui/qstandarditemmodel.h>

#include "Widgets/Inspectors/InspectorWidgetBase.hpp"

using namespace Poly;

class CustomTree : public QTreeWidget
{
	Q_OBJECT

public:
	CustomTree(QWidget* parent) : QTreeWidget(parent) {	}

signals:
	void Dropped(Dynarray<QTreeWidgetItem*> droppedItems);

private:
	void dropEvent(QDropEvent* e) override 
	{ 
		Dynarray<QTreeWidgetItem*> droppedItems;

		for (QTreeWidgetItem* i : selectedItems())
			droppedItems.PushBack(i);

		QTreeWidget::dropEvent(e);  

		emit Dropped(droppedItems);
	}
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


public slots:
	// Sets currently viewed world to given object and updates inspector.
	void WorldChanged(World* world);

	// Adds this new entity to tree.
	void EntitiesSpawned(Dynarray<Entity*> entities);

	// removed items from list
	void EntitiesDestroyed();

	// Reparents all currently selected entities to new parent.
	void EntitiesReparented(Entity* parent);

	// Sets selected entities.
	void EntitiesSelectionChanged(Dynarray<Entity*> entities);

	// Updates fields within tree items.
	void SoftUpdate();


private:
	// Called recursively to add entity and all its chiildren to tree
	void AddEntityToTree(Entity* entity, QTreeWidgetItem* parent);

	// TODO(squares): reprace asserts to exceptions in engine and make use of them.
	World* WorldObj;
	Dynarray<Entity*> SelectedEntities;

	QMenu* ContextMenu;
		QAction* AddEntityAction;
		QAction* RemoveEntityAction;
		QAction* ReparentEntityAction;

	CustomTree* Tree;
	std::map<QTreeWidgetItem*, Entity*> ItemToEntity;

private slots:
	void SelectionChanged();
	void Drop(Dynarray<QTreeWidgetItem*> droppedItems);

	void SpawnContextMenu(QPoint pos);
		void SpawnEntities();
		void DestroyEntities();
		void ReparentEntities();
};