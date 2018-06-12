#include "PolyEditorPCH.hpp"

#include <sstream>

//------------------------------------------------------------------------------
WorldInspectorWidget::WorldInspectorWidget(QWidget* parent)
	: InspectorWidgetBase(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &EntityInspectorWidget::customContextMenuRequested, this, &WorldInspectorWidget::SpawnContextMenu);

	Tree = new QTreeWidget(this);
	Tree->setHeaderLabels(QStringList() << "" << "Visible" << "Name" << "ID");

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

	connect(Tree, &QTreeWidget::itemClicked, this, &WorldInspectorWidget::SelectionChanged);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::InitializeConnections()
{
	connect(gApp->InspectorMgr, &InspectorManager::EngineInitialized, this, &WorldInspectorWidget::SetObject);
	connect(gApp->InspectorMgr, &InspectorManager::EngineDeinitialized, this, &WorldInspectorWidget::Reset);

	connect(gApp->InspectorMgr->ViewportInspector, &ViewportInspectorWidget::EntitiesSelectionChanged, this, &WorldInspectorWidget::SetSelectedEntities);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::Reset()
{
	World = nullptr;
	Tree->clear();
	EntityFromItem.clear();
	SelectedEntities.Clear();
	emit EntitiesSelectionChanged(SelectedEntities);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SetObject(::World* world)
{
	World = world;

	ReloadInspector();
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SetSelectedEntities(Dynarray<Entity*> entities)
{
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::UpdateInspector()
{
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::ReloadInspector()
{
	Tree->clear();
	EntityFromItem.clear();

	QTreeWidgetItem* root = new QTreeWidgetItem(Tree);

	std::stringstream ss;
	ss << World->GetRoot()->GetID();
	root->setText(3, (&ss.str()[0]));
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
	entityTree->setText(3, (&ss.str()[0]));
	entityTree->setCheckState(0, Qt::Checked);
	EntityFromItem.insert(std::pair<QTreeWidgetItem*, Entity*>(entityTree, entity));

	for (auto child : entity->GetChildren())
		AddEntityToTree(child, entityTree);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SelectionChanged(QTreeWidgetItem* item, int column)
{
	SelectedEntities.Clear();

	for (QTreeWidgetItem* i : Tree->selectedItems())
		SelectedEntities.PushBack(EntityFromItem[item]);

	emit EntitiesSelectionChanged(SelectedEntities);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SpawnContextMenu(QPoint pos)
{
	ContextMenu->popup(this->mapToGlobal(pos));
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::AddEntity()
{
	AddEntityDialog dialog(World, EntityFromItem[Tree->selectedItems()[0]]);
	dialog.exec();

	if (!dialog.OperationCanceled())
		AddEntityToTree(dialog.GetResult(), Tree->selectedItems()[0]);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::RemoveEntity()
{
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::ReparentEntity()
{
}
