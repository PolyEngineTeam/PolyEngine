#include "PolyEditorPCH.hpp"

#include <sstream>
#include <QtWidgets/qmessagebox.h>

//		general
//------------------------------------------------------------------------------
WorldInspectorWidget::WorldInspectorWidget(QWidget* parent, InspectorManager* mgr)
	: InspectorWidgetBase(parent, mgr)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &EntityInspectorWidget::customContextMenuRequested, this, &WorldInspectorWidget::SpawnContextMenu);

	Tree = new CustomTree(this);
	Tree->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	Tree->setHeaderLabels(QStringList() << "Visible" << "Name" << "ID");
	Tree->setAcceptDrops(true);
	Tree->setDragEnabled(true);
	Tree->setDragDropMode(QAbstractItemView::InternalMove);

	setLayout(new QGridLayout(this));
	layout()->addWidget(Tree);

	// context menu
	ContextMenu = new QMenu(this);

		AddEntityAction = new QAction("Spawn Entities", this);
		ContextMenu->addAction(AddEntityAction);
		connect(AddEntityAction, &QAction::triggered, this, &WorldInspectorWidget::SpawnEntities);

		RemoveEntityAction = new QAction("Destroy Entities", this);
		ContextMenu->addAction(RemoveEntityAction);
		connect(RemoveEntityAction, &QAction::triggered, this, &WorldInspectorWidget::DestroyEntities);

		ReparentEntityAction = new QAction("Reparent Entities", this);
		ContextMenu->addAction(ReparentEntityAction);
		connect(ReparentEntityAction, &QAction::triggered, this, &WorldInspectorWidget::ReparentEntities);

	connect(Tree, &QTreeWidget::itemSelectionChanged, this, &WorldInspectorWidget::SelectionChanged);
	connect(Tree, &CustomTree::Dropped, this, &WorldInspectorWidget::Drop);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::InitializeConnections()
{
	connect(gApp->InspectorMgr, &InspectorManager::WorldChanged, this, &WorldInspectorWidget::WorldChanged);

	connect(gApp->InspectorMgr, &InspectorManager::EntitiesSpawned, this, &WorldInspectorWidget::EntitiesSpawned);
	connect(gApp->InspectorMgr, &InspectorManager::EntitiesDestroyed, this, &WorldInspectorWidget::EntitiesDestroyed);
	connect(gApp->InspectorMgr, &InspectorManager::EntitiesModified, this, &WorldInspectorWidget::Update);
	connect(gApp->InspectorMgr, &InspectorManager::EntitiesReparented, this, &WorldInspectorWidget::EntitiesReparented);
	connect(gApp->InspectorMgr, &InspectorManager::EntitiesSelectionChanged, this, &WorldInspectorWidget::EntitiesSelectionChanged);
	
	connect(gApp->InspectorMgr, &InspectorManager::Update, this, &WorldInspectorWidget::Update);
	connect(gApp->InspectorMgr, &InspectorManager::Reload, this, &WorldInspectorWidget::Reload);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::Reset()
{
	Tree->clear();
	ItemToEntity.clear();
}



//		slots
//------------------------------------------------------------------------------
void WorldInspectorWidget::WorldChanged()
{
	Tree->clear();
	ItemToEntity.clear();

	if (WorldObj)
	{
		QTreeWidgetItem* root = new QTreeWidgetItem(Tree);

		std::stringstream ss;
		ss << WorldObj->GetRoot()->GetID();
		root->setText(3, (&ss.str()[0]));
		ItemToEntity.insert(std::pair<QTreeWidgetItem*, Entity*>(root, WorldObj->GetRoot()));

		for (auto child : WorldObj->GetRoot()->GetChildren())
			AddEntityToTree(child, root);
	}
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::EntitiesSpawned()
{
	for (Entity* e : SelectedEntities)
	{
		Entity* parent = e->GetParent();

		for (auto i : ItemToEntity)
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
		for (auto i : ItemToEntity)
			if (i.second == e)
			{
				i.first->parent()->removeChild(i.first);
				ItemToEntity.erase(i.first);
				break;
			}
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::EntitiesReparented()
{
	QTreeWidgetItem* parentWidget;


	for (auto i : ItemToEntity)
		if (i.second == SelectedEntities[0]->GetParent())
		{
			parentWidget = i.first;
			break;
		}

	int counter = 0;
	for (Entity* e : SelectedEntities)
		for (auto i : ItemToEntity)
			if (i.second == e)
			{
				i.first->parent()->takeChild(i.first->parent()->indexOfChild(i.first));
				parentWidget->insertChild(counter, i.first);
				break;
			}
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::EntitiesSelectionChanged()
{
	DisableSelectionChangedSlot = true;
	Tree->clearSelection();

	for (Entity* e : SelectedEntities)
		for (auto i : ItemToEntity)
			if (i.second == e)
			{
				Tree->setItemSelected(i.first, true);
				break;
			}

	DisableSelectionChangedSlot = false;
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::Update()
{
	for (auto i : ItemToEntity)
	{
		std::stringstream ss;
		ss << i.second->GetID();
		i.first->setText(3, (&ss.str()[0]));
		i.first->setText(2, i.second->Name.GetCStr());
	}
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::Reload()
{
	EntitiesSelectionChanged();
}



//		internal
//------------------------------------------------------------------------------
void WorldInspectorWidget::AddEntityToTree(Entity* entity, QTreeWidgetItem* parent)
{
	QTreeWidgetItem* entityTree = new QTreeWidgetItem(parent);

	std::stringstream ss;
	ss << entity->GetID();
	entityTree->setText(2, (&ss.str()[0]));
	entityTree->setText(1, entity->Name.GetCStr());
	entityTree->setCheckState(0, Qt::Checked);
	ItemToEntity.insert(std::pair<QTreeWidgetItem*, Entity*>(entityTree, entity));

	for (auto child : entity->GetChildren())
		AddEntityToTree(child, entityTree);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SpawnContextMenu(QPoint pos)
{
	ContextMenu->popup(this->mapToGlobal(pos));
}



//		functions modifying state (should emit events)
//------------------------------------------------------------------------------
void WorldInspectorWidget::SelectionChanged()
{
	if (DisableSelectionChangedSlot)
		return;

	Dynarray<Entity*> entities;

	for (QTreeWidgetItem* i : Tree->selectedItems())
		entities.PushBack(ItemToEntity[i]);

	gApp->InspectorMgr->EntitiesSelectionChangedSlot(entities);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::Drop(Dynarray<QTreeWidgetItem*> droppedItems)
{
	Entity* parent = ItemToEntity[droppedItems[0]->parent()];

	Dynarray<Entity*> ents;

	for (QTreeWidgetItem* i : droppedItems)
	{
		Entity* e = ItemToEntity[i];
		e->SetParent(parent);
		ents.PushBack(e);
	}

	gApp->InspectorMgr->EntitiesSelectionChangedSlot(ents);
	gApp->InspectorMgr->EntitiesReparentedSlot();
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SpawnEntities()
{
	if (!WorldObj)
	{
		QMessageBox msgBox;
		msgBox.setText("Can't spawn entities without world.");
		msgBox.exec();
		
		return;
	}

	EntityDialog dialog;
	Dynarray<Entity*> result = dialog.SpawnEntities(WorldObj, SelectedEntities);

	if (!dialog.OperationCanceled()) 
	{
		Manager->EntitiesSelectionChangedSlot(result);
		Manager->EntitiesSpawnedSlot();
	}

}

//------------------------------------------------------------------------------
void WorldInspectorWidget::DestroyEntities()
{
	if (!WorldObj)
	{
		QMessageBox msgBox;
		msgBox.setText("Can't destroy entities without world.");
		msgBox.exec();

		return;
	}

	EntityDialog dialog;
	dialog.DestroyEntities(WorldObj, SelectedEntities);

	if (!dialog.OperationCanceled())
		Manager->EntitiesDestroyedSlot();
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::ReparentEntities()
{
	if (!WorldObj)
	{
		QMessageBox msgBox;
		msgBox.setText("Can't reparent entities without selected world.");
		msgBox.exec();

		return;
	}

	EntityDialog dialog;
	Dynarray<Entity*> result = dialog.ReparentEntities(WorldObj, SelectedEntities);

	if (!dialog.OperationCanceled())
	{
		Manager->EntitiesSelectionChangedSlot(result);
		Manager->EntitiesReparentedSlot();
	}
}
