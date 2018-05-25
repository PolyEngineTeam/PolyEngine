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

	// Updates content of the inspector.
	void UpdateViewer();

signals:
	// When entity is double clicked then this signal is emitted.
	void EntitySelected(Entity* entity);

private:
	void AddEntityToTree(QTreeWidgetItem* parent, Entity* entity);

	World* World;

	std::unique_ptr<QTreeWidget> Tree;
	std::map<QTreeWidgetItem*, Entity*> EntityFromID;

private slots:
	void SelectionChanged(QTreeWidgetItem* item, int column);
};