#pragma once

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QLayout>
#include <QLabel>

#include "PolyWidget.hpp"

using namespace Poly;

class WorldExplorerWidget : public PolyWidget
{
public:
	WorldExplorerWidget(const QString& title, QWidget* parent);

	void SetWorld(World* world);
	//Entity* GetSelectedEntity();

signals:
	void EntitySelected();

private:
	std::unique_ptr<QLayout> Layout;
	std::unique_ptr<QTreeWidget> Tree;

	World* World;
};