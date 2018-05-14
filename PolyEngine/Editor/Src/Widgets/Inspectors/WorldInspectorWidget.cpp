#include "PolyEditorPCH.hpp"

WorldExplorerWidget::WorldExplorerWidget()
{
	Tree = std::make_unique<QTreeWidget>(this);
	Tree->setHeaderLabels(QStringList() << "Entity ID");

	setLayout(new QGridLayout(this));
	layout()->addWidget(Tree.get());

	connect(Tree.get(), &QTreeWidget::itemDoubleClicked, this, &WorldExplorerWidget::SelectionChanged);
}

void WorldExplorerWidget::SetWorld(::World* world)
{
	World = world;

	UpdateViewer();
}

void WorldExplorerWidget::UpdateViewer()
{
	Tree->clear();

	QTreeWidgetItem* entityTree = new QTreeWidgetItem(Tree.get());
	int id = (int)World->GetRoot()->GetID().GetHash();
	entityTree->setText(0, String::From(id).GetCStr());
	EntityFromID.insert(std::pair<int, Entity*>(id, World->GetRoot()));

	for (auto child : World->GetRoot()->GetChildren())
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
	emit EntitySelected(EntityFromID[item->text(0).toInt()]);
}
