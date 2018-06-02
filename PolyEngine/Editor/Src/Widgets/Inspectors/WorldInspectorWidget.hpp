#pragma once

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>
#include <UniqueID.hpp>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QLayout>
#include <QLabel>

#include "Widgets/PolyWidget.hpp"

using namespace Poly;

// Displays tree of entities in the world. Enables adding, removing and reparenting entities.
class WorldInspectorWidget : public PolyWidget
{
	Q_OBJECT

public:
	WorldInspectorWidget(QWidget* parent);

	// Sets currently viewed world to given object and updates inspector.
	void SetObject(World* world);

	// Removes all entities from list and loads them again.
	void ReloadInspector();

signals:
	// When entity is double clicked then this signal is emitted.
	void EntitySelected(Entity* entity);

	// When we don't want to display any entity or engine is deinitialized.
	void EntityDeselected();

private:
	void AddEntityToTree(Entity* entity, QTreeWidgetItem* parent);

	World* World;

	QMenu* ContextMenu;
		QAction* AddEntityAction;
		QAction* RemoveEntityAction;
		QAction* ReparentEntityAction;

	QTreeWidget* Tree;
	std::map<QTreeWidgetItem*, Entity*> EntityFromID;

private slots:
	void SelectionChanged(QTreeWidgetItem* item, int column);
	void SpawnContextMenu(QPoint pos);
		void AddEntity();
		void RemoveEntity();
		void ReparentEntity();

	// Removes all entities from list.
	void Reset();
};