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
		connect(RemoveEntityAction, &QAction::triggered, this, &WorldInspectorWidget::GetEntitiesToDestroy);

		ReparentEntityAction = new QAction("Reparent Entities", this);
		ContextMenu->addAction(ReparentEntityAction);
		connect(ReparentEntityAction, &QAction::triggered, this, &WorldInspectorWidget::ReparentEntities);

	connect(Tree, &QTreeWidget::itemSelectionChanged, this, &WorldInspectorWidget::SelectionChanged);
	connect(Tree, &CustomTree::Dropped, this, &WorldInspectorWidget::Drop);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::InitializeConnections()
{
	connect(Manager, &InspectorManager::WorldChanged, this, &WorldInspectorWidget::WorldChanged);

	connect(Manager, &InspectorManager::EntitiesSpawned, this, &WorldInspectorWidget::EntitiesSpawned);
	connect(Manager, &InspectorManager::EntitiesDestroyed, this, &WorldInspectorWidget::EntitiesDestroyed);
	connect(Manager, &InspectorManager::EntitiesModified, this, &WorldInspectorWidget::Update);
	connect(Manager, &InspectorManager::EntitiesReparented, this, &WorldInspectorWidget::EntitiesReparented);
	connect(Manager, &InspectorManager::EntitiesSelectionChanged, this, &WorldInspectorWidget::EntitiesSelectionChanged);
	
	connect(Manager, &InspectorManager::Update, this, &WorldInspectorWidget::Update);
	connect(Manager, &InspectorManager::Reload, this, &WorldInspectorWidget::Reload);
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

	if (Manager->GetScene())
	{
		QTreeWidgetItem* root = new QTreeWidgetItem(Tree);

		std::stringstream ss;
		ss << Manager->GetScene()->GetRoot()->GetUUID();
		root->setText(2, (&ss.str()[0]));
		root->setText(1, Manager->GetScene()->GetRoot()->GetName().GetCStr());
		root->setCheckState(0, Qt::Checked);
		ItemToEntity.insert(std::pair<QTreeWidgetItem*, Entity*>(root, Manager->GetScene()->GetRoot()));

		for (auto& child : Manager->GetScene()->GetRoot()->GetChildren())
			AddEntityToTree(child.get(), root);
	}
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::EntitiesSpawned()
{
	for (Entity* e : Manager->GetSelectedEntities())
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
	DisableSelectionChangedSlot = true;

	for (Entity* e : Manager->GetSelectedEntities())
		for (auto i : ItemToEntity)
			if (i.second == e)
			{
				i.first->parent()->removeChild(i.first);
				ItemToEntity.erase(i.first);
				break;
			}

	DisableSelectionChangedSlot = false;
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::EntitiesReparented()
{
	QTreeWidgetItem* parentWidget;


	for (auto i : ItemToEntity)
		if (i.second == Manager->GetSelectedEntities()[0]->GetParent())
		{
			parentWidget = i.first;
			break;
		}

	int counter = 0;
	for (Entity* e : Manager->GetSelectedEntities())
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

	for (Entity* e : Manager->GetSelectedEntities())
		for (auto i : ItemToEntity)
			if (i.second == e)
			{
				i.first->setSelected(true);
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
		ss << i.second->GetUUID();
		i.first->setText(2, (&ss.str()[0]));
		i.first->setText(1, i.second->GetName().GetCStr());
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
	ss << entity->GetUUID();
	entityTree->setText(2, (&ss.str()[0]));
	entityTree->setText(1, entity->GetName().GetCStr());
	entityTree->setCheckState(0, Qt::Checked);
	ItemToEntity.insert(std::pair<QTreeWidgetItem*, Entity*>(entityTree, entity));

	for (auto& child : entity->GetChildren())
		AddEntityToTree(child.get(), entityTree);
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SpawnContextMenu(QPoint pos)
{
	if (Manager->GetEngine())
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

	Manager->EntitiesSelectionChangedSlot(entities);
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

	Manager->EntitiesSelectionChangedSlot(ents);
	Manager->EntitiesReparentedSlot();
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::SpawnEntities()
{
	if (!Manager->GetScene())
	{
		QMessageBox msgBox;
		msgBox.setText("Can't spawn entities without world.");
		msgBox.exec();
		
		return;
	}

	EntityDialog dialog;
	Dynarray<Entity*> result = dialog.SpawnEntities(Manager->GetScene(), Manager->GetSelectedEntities());

	if (!dialog.OperationCanceled()) 
	{
		Manager->EntitiesSelectionChangedSlot(result);
		Manager->EntitiesSpawnedSlot();
	}

}

//------------------------------------------------------------------------------
void WorldInspectorWidget::GetEntitiesToDestroy()
{
	DisableSelectionChangedSlot = true;

	if (!Manager->GetScene())
	{
		QMessageBox msgBox;
		msgBox.setText("Can't destroy entities without world.");
		msgBox.exec();

		return;
	}

	EntityDialog dialog;
	Dynarray<Entity*> result = dialog.GetEntitiesToDestroy(Manager->GetScene(), Manager->GetSelectedEntities());

	if (!dialog.OperationCanceled())
	{
		Manager->EntitiesSelectionChangedSlot(result);
		Manager->EntitiesDestroyedSlot();
	}
	DisableSelectionChangedSlot = false;
}

//------------------------------------------------------------------------------
void WorldInspectorWidget::ReparentEntities()
{
	if (!Manager->GetScene())
	{
		QMessageBox msgBox;
		msgBox.setText("Can't reparent entities without selected world.");
		msgBox.exec();

		return;
	}

	EntityDialog dialog;
	Dynarray<Entity*> result = dialog.ReparentEntities(Manager->GetScene(), Manager->GetSelectedEntities());

	if (!dialog.OperationCanceled())
	{
		Manager->EntitiesSelectionChangedSlot(result);
		Manager->EntitiesReparentedSlot();
	}
}
