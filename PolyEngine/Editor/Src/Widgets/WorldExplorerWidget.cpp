#include "PolyEditorPCH.hpp"

WorldExplorerWidget::WorldExplorerWidget()
{
	Tree = std::make_unique<QTreeWidget>(this);
	Tree->move(12, 12);
	Tree->resize(400, 250);
	Tree->setHeaderLabels(QStringList() << "Entity ID");

	connect(Tree.get(), &QTreeWidget::itemDoubleClicked, this, &WorldExplorerWidget::SelectionChanged);
}

void WorldExplorerWidget::SetWorld(::World* world)
{
	if (World)
		Tree->clear();
	
	World = world;

	QTreeWidgetItem* entityTree = new QTreeWidgetItem(Tree.get());
	int id = (int)world->GetRoot()->GetID().GetHash();
	entityTree->setText(0, String::From(id).GetCStr());
	EntityFromID.insert(std::pair<int, Entity*>(id, world->GetRoot()));

	for (auto child : world->GetRoot()->GetChildren())
		AddEntityToTree(entityTree, child);
}

void WorldExplorerWidget::AddEntityToTree(QTreeWidgetItem* parent, Entity* entity)
{
	QTreeWidgetItem* entityTree = new QTreeWidgetItem(parent);
	int id = (int)entity->GetID().GetHash();
	entityTree->setText(0, String::From(id).GetCStr());
	EntityFromID.insert(std::pair<int, Entity*>(id, entity));

	for (auto child : entity->GetChildren())
		AddEntityToTree(entityTree, child);
}

void WorldExplorerWidget::SelectionChanged(QTreeWidgetItem* item, int column)
{
	emit EntitySelected(EntityFromID[item->text(0).toUInt()]);
}
