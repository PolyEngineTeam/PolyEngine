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

		AddEntityAction = new QAction("Add Entities", this);
		ContextMenu->addAction(AddEntityAction);
		connect(AddEntityAction, &QAction::triggered, this, &WorldInspectorWidget::SpawnEntities);

		RemoveEntityAction = new QAction("Remove Entities", this);
		ContextMenu->addAction(RemoveEntityAction);
		connect(RemoveEntityAction, &QAction::triggered, this, &WorldInspectorWidget::DestroyEntities);

		ReparentEntityAction = new QAction("Reparent Entities", this);
		ContextMenu->addAction(ReparentEntityAction);
		connect(ReparentEntityAction, &QAction::triggered, this, &WorldInspectorWidget::ReparentEntities);

	connect(Tree, &QTreeWidget::itemClicked, this, &WorldInspectorWidget::SelectionChanged);
	connect(Tree, &CustomTree::Dropped, this, &WorldInspectorWidget::Drop);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::InitializeConnections()
{
	connect(gApp->InspectorMgr, &InspectorManager::WorldChanged, this, &WorldInspectorWidget::WorldChanged);

	connect(gApp->InspectorMgr, &InspectorManager::EntitiesSpawned, this, &WorldInspectorWidget::EntitiesSpawned);
	connect(gApp->InspectorMgr, &InspectorManager::EntitiesDestroyed, this, &WorldInspectorWidget::EntitiesDestroyed);
	connect(gApp->InspectorMgr, &InspectorManager::EntitiesModified, this, &WorldInspectorWidget::SoftUpdate);
	connect(gApp->InspectorMgr, &InspectorManager::EntitiesReparented, this, &WorldInspectorWidget::EntitiesReparented);
	connect(gApp->InspectorMgr, &InspectorManager::EntitiesSelectionChanged, this, &WorldInspectorWidget::EntitiesSelectionChanged);
	
	connect(gApp->InspectorMgr, &InspectorManager::EntitiesModified, this, &WorldInspectorWidget::SoftUpdate);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::Reset()
{
	WorldObj = nullptr;
	SelectedEntities.Clear();

	Tree->clear();
	EntityFromItem.clear();

	emit gApp->InspectorMgr->EntitiesSelectionChanged(SelectedEntities);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::WorldChanged(::World* world)
{
	WorldObj = nullptr;
	SelectedEntities.Clear();

	Tree->clear();
	EntityFromItem.clear();

	WorldObj = world;

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
void WorldInspectorWidget::EntitiesSpawned(Dynarray<Entity*> entities)
{
	for (Entity* e : entities)
	{
		Entity* parent = e->GetParent();

		for (auto i : EntityFromItem)
			if (i.second == parent)
			{
				AddEntityToTree(e, i.first);
				break;
			}
	}
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::EntitiesDestroyed()
{
	for (Entity* e : SelectedEntities)
		for (auto i : EntityFromItem)
			if (i.second == e)
			{
				i.first->parent()->removeChild(i.first);
				EntityFromItem.erase(i.first);
				break;
			}

	SelectedEntities.Clear();
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::EntitiesReparented(Entity* parent)
{
	QTreeWidgetItem* parentWidget;

	for (auto i : EntityFromItem)
		if (i.second == parent)
			parentWidget = i.first;

	for (Entity* e : SelectedEntities)
		for (auto i : EntityFromItem)
			if (i.second == e)
			{
				parentWidget->addChild(i.first);
				break;
			}
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::EntitiesSelectionChanged(Dynarray<Entity*> entities)
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
void WorldInspectorWidget::SoftUpdate()
{
	for (auto i : EntityFromItem)
	{
		std::stringstream ss;
		ss << i.second->GetID();
		i.first->setText(3, (&ss.str()[0]));
		i.first->setText(2, i.second->Name.GetCStr());
	}
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::AddEntityToTree(Entity* entity, QTreeWidgetItem* parent)
{
	QTreeWidgetItem* entityTree = new QTreeWidgetItem(parent);

	std::stringstream ss;
	ss << entity->GetID();
	entityTree->setText(3, (&ss.str()[0]));
	entityTree->setText(2, entity->Name.GetCStr());
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
void WorldInspectorWidget::Drop(QDropEvent* e)
{
	if (!Tree->indexAt(e->pos()).isValid())
		return;

	Entity* parent = EntityFromItem[Tree->itemAt(e->pos())];

	// FIXME(squares): problems with reparenting: "parenting cycle" and "parenting myself"
	for (Entity* entity : SelectedEntities)
		entity->SetParent(parent);

	emit gApp->InspectorMgr->EntitiesReparented(parent);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SpawnContextMenu(QPoint pos)
{
	ContextMenu->popup(this->mapToGlobal(pos));
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SpawnEntities()
{
	EntityDialog dialog;
	Dynarray<Entity*> result = dialog.SpawnEntities(WorldObj, SelectedEntities);

	if (!dialog.OperationCanceled())
		emit gApp->InspectorMgr->EntitiesSpawned(result);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::DestroyEntities()
{
	EntityDialog dialog;
	dialog.DestroyEntities(SelectedEntities);

	emit gApp->InspectorMgr->EntitiesDestroyed();
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::ReparentEntities()
{
	EntityDialog dialog;
	Entity* result = dialog.ReparentEntities(SelectedEntities);
	
	emit gApp->InspectorMgr->EntitiesReparented(result);
}
