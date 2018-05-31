#include "PolyEditorPCH.hpp"

#include <sstream>

//------------------------------------------------------------------------------
WorldInspectorWidget::WorldInspectorWidget(QWidget* parent)
	: PolyWidget(parent)
{
	connect(&gApp->EngineMgr, &EngineManager::Initialized, this, &WorldInspectorWidget::SetObject);
	connect(&gApp->EngineMgr, &EngineManager::Deinitialized, this, &WorldInspectorWidget::Reset);

	Tree = new QTreeWidget(this);
	Tree->setHeaderLabels(QStringList() << "Visible" << "Name" << "ID");

	setLayout(new QGridLayout(this));
	layout()->addWidget(Tree);

	connect(Tree, &QTreeWidget::itemDoubleClicked, this, &WorldInspectorWidget::SelectionChanged);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SetObject(::World* world)
{
	World = world;

	ReloadInspector();
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::ReloadInspector()
{
	Tree->clear();
	EntityFromID.clear();

	for (auto child : World->GetRoot()->GetChildren())
		AddEntityToTree(child);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::AddEntityToTree(Entity* entity, QTreeWidgetItem* parent)
{
	QTreeWidgetItem* entityTree;

	if (parent == nullptr)
		entityTree = new QTreeWidgetItem(Tree);
	else
		entityTree = new QTreeWidgetItem(parent);

	std::stringstream ss;
	ss << entity->GetID();
	entityTree->setText(0, (&ss.str()[0]));
	EntityFromID.insert(std::pair<QTreeWidgetItem*, Entity*>(entityTree, entity));

	for (auto child : entity->GetChildren())
		AddEntityToTree(child, entityTree);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SelectionChanged(QTreeWidgetItem* item, int column)
{
	emit EntitySelected(EntityFromID[item]);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::Reset()
{
	World = nullptr;
	Tree->clear();
	EntityFromID.clear();
	emit EntityDeselected();
}
