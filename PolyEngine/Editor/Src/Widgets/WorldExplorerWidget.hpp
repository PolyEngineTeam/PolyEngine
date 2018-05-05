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

class WorldExplorerWidget : public PolyWidget
{
	Q_OBJECT

public:
	WorldExplorerWidget();

	void SetWorld(World* world);
	void UpdateViewer();

signals:
	void EntitySelected(Entity* entity);

private:
	void AddEntityToTree(QTreeWidgetItem* parent, Entity* entity);

	World* World;

	std::unique_ptr<QTreeWidget> Tree;
	//TODO(squares): use size_t keys
	std::map<int, Entity*> EntityFromID;

private slots:
	void SelectionChanged(QTreeWidgetItem* item, int column);
};