#include "PolyEditorPCH.hpp"

#include <sstream>

//------------------------------------------------------------------------------
WorldInspectorWidget::WorldInspectorWidget(QWidget* parent)
	: PolyWidget(parent)
{
	connect(gApp->InspectorMgr, &InspectorManager::EngineInitialized, this, &WorldInspectorWidget::SetObject);
	connect(gApp->InspectorMgr, &InspectorManager::EngineDeinitialized, this, &WorldInspectorWidget::Reset);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &EntityInspectorWidget::customContextMenuRequested, this, &WorldInspectorWidget::SpawnContextMenu);

	Tree = new QTreeWidget(this);
	Tree->setHeaderLabels(QStringList() << "Visible" << "Name" << "ID");

	setLayout(new QGridLayout(this));
	layout()->addWidget(Tree);

	// context menu
	ContextMenu = new QMenu(this);

		AddEntityAction = new QAction("Add Entity", this);
		ContextMenu->addAction(AddEntityAction);
		connect(AddEntityAction, &QAction::triggered, this, &WorldInspectorWidget::AddEntity);

		RemoveEntityAction = new QAction("Remove Entity", this);
		ContextMenu->addAction(RemoveEntityAction);
		connect(RemoveEntityAction, &QAction::triggered, this, &WorldInspectorWidget::RemoveEntity);

		ReparentEntityAction = new QAction("Reparent Entity", this);
		ContextMenu->addAction(ReparentEntityAction);
		connect(ReparentEntityAction, &QAction::triggered, this, &WorldInspectorWidget::ReparentEntity);

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
	EntityFromItem.clear();

	QTreeWidgetItem* root = new QTreeWidgetItem(Tree);

	std::stringstream ss;
	ss << World->GetRoot()->GetID();
	root->setText(2, (&ss.str()[0]));
	EntityFromItem.insert(std::pair<QTreeWidgetItem*, Entity*>(root, World->GetRoot()));

	for (auto child : World->GetRoot()->GetChildren())
		AddEntityToTree(child, root);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::AddEntityToTree(Entity* entity, QTreeWidgetItem* parent)
{
	QTreeWidgetItem* entityTree = new QTreeWidgetItem(parent);

	std::stringstream ss;
	ss << entity->GetID();
	entityTree->setText(2, (&ss.str()[0]));
	EntityFromItem.insert(std::pair<QTreeWidgetItem*, Entity*>(entityTree, entity));

	for (auto child : entity->GetChildren())
		AddEntityToTree(child, entityTree);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SelectionChanged(QTreeWidgetItem* item, int column)
{
	Entity* e = EntityFromItem[item];

	if (e != World->GetRoot())
		emit EntitySelected(e);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SpawnContextMenu(QPoint pos)
{
	ContextMenu->popup(this->mapToGlobal(pos));
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::AddEntity()
{
	AddEntityDialog dialog(World, EntityFromItem[Tree->itemAt(QCursor::pos())]);
	dialog.exec();
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::RemoveEntity()
{
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::ReparentEntity()
{
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::Reset()
{
	World = nullptr;
	Tree->clear();
	EntityFromItem.clear();
	emit EntityDeselected();
}
