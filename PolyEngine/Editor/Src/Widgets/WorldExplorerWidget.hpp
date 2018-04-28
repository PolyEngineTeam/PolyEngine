#pragma once

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>
#include <UniqueID.hpp>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QLayout>
#include <QLabel>

#include "Windows/PolyDockWindow.hpp"

using namespace Poly;

class WorldExplorerWidget : public PolyDockWindow
{
	Q_OBJECT

public:
	WorldExplorerWidget(const QString& title, QWidget* parent);

	void SetWorld(World* world);

signals:
	void EntitySelected(Entity* entity);

private:
	void AddEntityToTree(QTreeWidgetItem* parent, Entity* entity);

	std::unique_ptr<QLayout> Layout;
	std::unique_ptr<QTreeWidget> Tree;
	//TODO(squares): use size_t keys
	std::map<int, Entity*> EntityFromID;

	World* World;

private slots:
	void SelectionChanged(QTreeWidgetItem* item, int column);
};