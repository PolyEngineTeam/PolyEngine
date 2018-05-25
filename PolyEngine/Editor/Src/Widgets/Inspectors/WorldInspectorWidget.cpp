#include "PolyEditorPCH.hpp"

#include <sstream>

//------------------------------------------------------------------------------
WorldInspectorWidget::WorldInspectorWidget(QWidget* parent)
	: PolyWidget(parent)
{
	connect(&gApp->EngineMgr, &EngineManager::Initialized, this, &WorldInspectorWidget::SetObject);
	connect(&gApp->EngineMgr, &EngineManager::Deinitialized, this, &WorldInspectorWidget::Reset);

	Tree = std::make_unique<QTreeWidget>(this);
	Tree->setHeaderLabels(QStringList() << "Entity ID");

	setLayout(new QGridLayout(this));
	layout()->addWidget(Tree.get());

	connect(Tree.get(), &QTreeWidget::itemDoubleClicked, this, &WorldInspectorWidget::SelectionChanged);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SetObject(::World* world)
{
	World = world;

	UpdateInspector();
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::UpdateInspector()
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
		entityTree = new QTreeWidgetItem(Tree.get());
	else
		entityTree = new QTreeWidgetItem(parent);

	std::stringstream ss;
	ss << entity->GetID();
	entityTree->setText(0, (&ss.str()[0]));
	EntityFromID.insert(std::pair<QTreeWidgetItem*, Entity*>(entityTree, entity));

	for (auto child : entity->GetChildren())
		AddEntityToTree(child, entityTree);
}

void WorldInspectorWidget::Reset()
{
	Tree->clear();
	EntityFromID.clear();
	emit EntityDeselected();
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SelectionChanged(QTreeWidgetItem* item, int column)
{
	emit EntitySelected(EntityFromID[item]);
}
