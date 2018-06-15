#include "PolyEditorPCH.hpp"

#include <sstream>

//------------------------------------------------------------------------------
WorldInspectorWidget::WorldInspectorWidget(QWidget* parent)
	: InspectorWidgetBase(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &EntityInspectorWidget::customContextMenuRequested, this, &WorldInspectorWidget::SpawnContextMenu);

	Tree = new CustomTree(this);
	Tree->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	Tree->setHeaderLabels(QStringList() << "" << "Visible" << "Name" << "ID");
	Tree->setAcceptDrops(true);
	Tree->setDragEnabled(true);
	Tree->setDragDropMode(QAbstractItemView::InternalMove);

	setLayout(new QGridLayout(this));
	layout()->addWidget(Tree);

	// context menu
	ContextMenu = new QMenu(this);

		AddEntityAction = new QAction("Add Entity", this);
		ContextMenu->addAction(AddEntityAction);
		connect(AddEntityAction, &QAction::triggered, this, &WorldInspectorWidget::AddEntity);

		RemoveEntityAction = new QAction("Remove Entities", this);
		ContextMenu->addAction(RemoveEntityAction);
		connect(RemoveEntityAction, &QAction::triggered, this, &WorldInspectorWidget::RemoveEntity);

		ReparentEntityAction = new QAction("Reparent Entities", this);
		ContextMenu->addAction(ReparentEntityAction);
		connect(ReparentEntityAction, &QAction::triggered, this, &WorldInspectorWidget::ReparentEntity);

	connect(Tree, &QTreeWidget::itemClicked, this, &WorldInspectorWidget::SelectionChanged);
	connect(Tree, &CustomTree::Dropped, this, &WorldInspectorWidget::EntitiesReparented);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::InitializeConnections()
{
	connect(gApp->InspectorMgr, &InspectorManager::EngineInitialized, this, &WorldInspectorWidget::SetObject);
	connect(gApp->InspectorMgr, &InspectorManager::EngineDeinitialized, this, &WorldInspectorWidget::Reset);

	connect(gApp->InspectorMgr, &InspectorManager::EntitiesSelectionChanged, this, &WorldInspectorWidget::SetSelectedEntities);
	connect(gApp->InspectorMgr, &InspectorManager::EntitiesReparented, this, &WorldInspectorWidget::ReloadInspector);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::Reset()
{
	WorldObj = nullptr;
	Tree->clear();
	EntityFromItem.clear();
	SelectedEntities.Clear();
	emit gApp->InspectorMgr->EntitiesSelectionChanged(SelectedEntities);
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
	ss << WorldObj->GetRoot()->GetID();
	root->setText(3, (&ss.str()[0]));
	EntityFromItem.insert(std::pair<QTreeWidgetItem*, Entity*>(root, WorldObj->GetRoot()));

	for (auto child : WorldObj->GetRoot()->GetChildren())
		AddEntityToTree(child, root);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SetObject(::World* world)
{
	WorldObj = world;

	ReloadInspector();
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SetSelectedEntities(Dynarray<Entity*> entities)
{
	SelectedEntities = entities;

	for (Entity* e : entities)
		for (auto i : EntityFromItem)
			if (i.second == e)
			{
				Tree->setItemSelected(i.first, true);
				break;
			}
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
		SelectedEntities.PushBack(EntityFromItem[i]);

	emit gApp->InspectorMgr->EntitiesSelectionChanged(SelectedEntities);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::EntitiesReparented(QDropEvent* e)
{
	if (!Tree->indexAt(e->pos()).isValid())
		return;

	Entity* parent = EntityFromItem[Tree->itemAt(e->pos())];

	for (Entity* e : SelectedEntities)
		e->SetParent(parent);

	emit gApp->InspectorMgr->EntitiesReparented(parent);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SpawnContextMenu(QPoint pos)
{
	ContextMenu->popup(this->mapToGlobal(pos));
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::AddEntity()
{
	AddEntityDialog dialog(WorldObj, EntityFromItem[Tree->selectedItems()[0]]);
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
