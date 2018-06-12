#pragma once

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>
#include <UniqueID.hpp>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QLayout>
#include <QLabel>

#include "Widgets/Inspectors/InspectorWidgetBase.hpp"

using namespace Poly;

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

	// Sets currently viewed world to given object and updates inspector.
	void SetObject(World* world);

	// Sets selected entities.
	void SetSelectedEntities(Dynarray<Entity*> entities);

	// TODO(squares): use controls for easy update
	// Updates names.
	void UpdateInspector();

	// Removes all entities from list and loads them again.
	void ReloadInspector();

signals:
	// When entity is clicked then this signal is emitted.
	void EntitiesSelectionChanged(Dynarray<Entity*> entities);

private:
	void AddEntityToTree(Entity* entity, QTreeWidgetItem* parent);

	World* World;
	Dynarray<Entity*> SelectedEntities;

	QMenu* ContextMenu;
		QAction* AddEntityAction;
		QAction* RemoveEntityAction;
		QAction* ReparentEntityAction;

	QTreeWidget* Tree;
	std::map<QTreeWidgetItem*, Entity*> EntityFromItem;

private slots:
	void SelectionChanged(QTreeWidgetItem* item, int column);
	void SpawnContextMenu(QPoint pos);
		void AddEntity();
		void RemoveEntity();
		void ReparentEntity();
};