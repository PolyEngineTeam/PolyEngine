#include "PolyEditorPCH.hpp"

#include <ECS/DeferredTaskSystem.hpp>

#include <sstream>
#include <QtWidgets/qmessagebox.h>

//------------------------------------------------------------------------------
Dynarray<Entity*> EntityDialog::SpawnEntities(World* world, Dynarray<Entity*> parents)
{
	setModal(true);
	Canceled = true;
	PredefinedEntities = parents;

	// create main layout
	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 1);
	MainLayout->setColumnStretch(1, 1);
	MainLayout->setColumnStretch(2, 1);

	// create potential parents list 
	EntitiesTree = new QTreeWidget(this);
	EntitiesTree->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	EntitiesTree->setHeaderLabels(QStringList() << "Name" << "ID");
	MainLayout->addWidget(EntitiesTree, 0, 0, 1, 3);

	for (auto child : world->GetRoot()->GetChildren())
		AddEntity(child);

	// create prefabs list
	PrefabTree = new QTreeWidget(this);
	PrefabTree->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	PrefabTree->setHeaderLabels(QStringList() << "Name" << "ID");
	MainLayout->addWidget(PrefabTree, 1, 0, 1, 3);
	AddPrefab("Empty entity");
	AddPrefab("SpikesLeft");
	AddPrefab("SpikesRight");
	AddPrefab("SpikesTop");
	AddPrefab("SpikesBottom");
	AddPrefab("Tile");
	AddPrefab("Powerup");

	// create and link buttons
	CancelButton = new QPushButton(this);
	CancelButton->setText("Cancel");
	connect(CancelButton, &QPushButton::clicked, this, &EntityDialog::Cancel);
	MainLayout->addWidget(CancelButton, 2, 0);

	OkButton = new QPushButton(this);
	OkButton->setText("Ok");
	connect(OkButton, &QPushButton::clicked, this, &EntityDialog::Ok);
	MainLayout->addWidget(OkButton, 2, 2);

	// input from user
	exec();

	Dynarray<Entity*> result;

	// apply
	if (!Canceled)
		for (auto i : EntitiesTree->selectedItems())
			for (auto p : PrefabTree->selectedItems())
				if (p->text(0) == "Empty entity")
				{
					Entity* e = DeferredTaskSystem::SpawnEntityImmediate(world);
					e->SetParent(ItemToEntity[i]);
					result.PushBack(e);
				}
				else
				{
					QMessageBox msgBox;
					msgBox.setText("\"" + p->text(0) + "is currently not supported, it will be skipped.");
					msgBox.exec();
				}

	return result;
}

//------------------------------------------------------------------------------
void EntityDialog::DestroyEntities(Dynarray<Entity*> parents)
{
	setModal(true);

	// create main layout
	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 1);
	MainLayout->setColumnStretch(1, 1);
	MainLayout->setColumnStretch(2, 1);

	// create potential parents list 
	EntitiesTree = new QTreeWidget(this);
	MainLayout->addWidget(EntitiesTree, 0, 0, 1, 3);

	//for (auto child : world->GetRoot()->GetChildren())
	//	AddEntity(child);

	// create prefabs list
	PrefabTree = new QTreeWidget(this);
	MainLayout->addWidget(EntitiesTree, 1, 0, 1, 3);
	AddPrefab("Empty entity");
	AddPrefab("SpikesLeft");
	AddPrefab("SpikesRight");
	AddPrefab("SpikesTop");
	AddPrefab("SpikesBottom");
	AddPrefab("Tile");
	AddPrefab("Powerup");

	// create and link buttons
	CancelButton = new QPushButton(this);
	CancelButton->setText("Cancel");
	connect(CancelButton, &QPushButton::clicked, this, &EntityDialog::Cancel);
	MainLayout->addWidget(CancelButton, 2, 0);

	OkButton = new QPushButton(this);
	OkButton->setText("Ok");
	connect(OkButton, &QPushButton::clicked, this, &EntityDialog::Ok);
	MainLayout->addWidget(OkButton, 2, 2);

	// input from user
	exec();

	// apply
}

//------------------------------------------------------------------------------
Entity* EntityDialog::ReparentEntities(Dynarray<Entity*> entities, Entity* parent)
{
	setModal(true);

	// create main layout
	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 1);
	MainLayout->setColumnStretch(1, 1);
	MainLayout->setColumnStretch(2, 1);

	// create potential parents list 
	EntitiesTree = new QTreeWidget(this);
	MainLayout->addWidget(EntitiesTree, 0, 0, 1, 3);

	//for (auto child : world->GetRoot()->GetChildren())
	//	AddEntity(child);

	// create prefabs list
	PrefabTree = new QTreeWidget(this);
	MainLayout->addWidget(EntitiesTree, 1, 0, 1, 3);
	AddPrefab("Empty entity");
	AddPrefab("SpikesLeft");
	AddPrefab("SpikesRight");
	AddPrefab("SpikesTop");
	AddPrefab("SpikesBottom");
	AddPrefab("Tile");
	AddPrefab("Powerup");

	// create and link buttons
	CancelButton = new QPushButton(this);
	CancelButton->setText("Cancel");
	connect(CancelButton, &QPushButton::clicked, this, &EntityDialog::Cancel);
	MainLayout->addWidget(CancelButton, 2, 0);

	OkButton = new QPushButton(this);
	OkButton->setText("Ok");
	connect(OkButton, &QPushButton::clicked, this, &EntityDialog::Ok);
	MainLayout->addWidget(OkButton, 2, 2);

	// input from user
	exec();

	// apply
	return nullptr;
}

//------------------------------------------------------------------------------
void EntityDialog::AddEntity(Entity* entity)
{
	QTreeWidgetItem* item = new QTreeWidgetItem(EntitiesTree);

	std::stringstream ss;
	ss << entity->GetID();
	item->setText(1, (&ss.str()[0]));
	item->setText(0, entity->Name.GetCStr());
	ItemToEntity.insert(std::pair<QTreeWidgetItem*, Entity*>(item, entity));

	for (Entity* e : PredefinedEntities)
		if (e == entity)
			item->setSelected(true);

	for (auto child : entity->GetChildren())
		AddEntity(child);
}

//------------------------------------------------------------------------------
void EntityDialog::AddPrefab(QString prefabName)
{
	QTreeWidgetItem* item = new QTreeWidgetItem(PrefabTree);

	item->setText(0, prefabName);
}

//------------------------------------------------------------------------------
void EntityDialog::Ok()
{
	Canceled = false;

	close();
}

//------------------------------------------------------------------------------
void EntityDialog::Cancel()
{
	close();
}
