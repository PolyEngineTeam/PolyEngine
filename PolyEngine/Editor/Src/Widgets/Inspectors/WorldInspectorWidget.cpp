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
	entityTree->setText(0, QString::number(World->GetRoot()->GetID().GetHash()));
	EntityFromID.insert(std::pair<QTreeWidgetItem*, Entity*>(entityTree, World->GetRoot()));

	for (auto child : World->GetRoot()->GetChildren())
		AddEntityToTree(entityTree, child);
}

void WorldExplorerWidget::AddEntityToTree(QTreeWidgetItem* parent, Entity* entity)
{
	QTreeWidgetItem* entityTree = new QTreeWidgetItem(parent);
	entityTree->setText(0, QString::number(entity->GetID().GetHash()));
	EntityFromID.insert(std::pair<QTreeWidgetItem*, Entity*>(entityTree, entity));

	for (auto child : entity->GetChildren())
		AddEntityToTree(entityTree, child);
}

void WorldExplorerWidget::SelectionChanged(QTreeWidgetItem* item, int column)
{
	emit EntitySelected(EntityFromID[item]);
}
