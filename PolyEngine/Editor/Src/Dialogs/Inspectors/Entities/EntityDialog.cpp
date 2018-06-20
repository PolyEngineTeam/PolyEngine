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

	FirstLabel = new QLabel(this);
	FirstLabel->setText("Choose where you want to spawn new entities.");
	MainLayout->addWidget(FirstLabel, 0, 0, 1, 3);

	// create potential parents list 
	EntitiesTree = new QTreeWidget(this);
	EntitiesTree->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	EntitiesTree->setHeaderLabels(QStringList() << "Name" << "ID");
	MainLayout->addWidget(EntitiesTree, 1, 0, 1, 3);

	for (auto child : world->GetRoot()->GetChildren())
		AddEntity(child);

	SecondLabel = new QLabel(this);
	SecondLabel->setText("Choose prefabs to spawn in each selected parent.");
	MainLayout->addWidget(SecondLabel, 2, 0, 1, 3);

	// create prefabs list
	PrefabTree = new QTreeWidget(this);
	PrefabTree->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	PrefabTree->setHeaderLabels(QStringList() << "Name" << "ID");
	MainLayout->addWidget(PrefabTree, 3, 0, 1, 3);
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
	MainLayout->addWidget(CancelButton, 4, 0);

	OkButton = new QPushButton(this);
	OkButton->setText("Spawn");
	connect(OkButton, &QPushButton::clicked, this, &EntityDialog::Ok);
	MainLayout->addWidget(OkButton, 4, 2);

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
					msgBox.setText("\"" + p->text(0) + "is currently not supported, it will be skipped for " +
						i->text(0) + " (" + i->text(1) + ").");
					msgBox.exec();
				}

	return result;
}

//------------------------------------------------------------------------------
void EntityDialog::DestroyEntities(World* world, Dynarray<Entity*> entities)
{
	setModal(true);
	Canceled = true;
	PredefinedEntities = entities;

	// create main layout
	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 1);
	MainLayout->setColumnStretch(1, 1);
	MainLayout->setColumnStretch(2, 1);

	FirstLabel = new QLabel(this);
	FirstLabel->setText("Choose entities to destroy (as well as all their children).");
	MainLayout->addWidget(FirstLabel, 0, 0, 1, 3);

	// create potential parents list 
	EntitiesTree = new QTreeWidget(this);
	EntitiesTree->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	EntitiesTree->setHeaderLabels(QStringList() << "Name" << "ID");
	MainLayout->addWidget(EntitiesTree, 1, 0, 1, 3);

	for (auto child : world->GetRoot()->GetChildren())
		AddEntity(child);

	// create and link buttons
	CancelButton = new QPushButton(this);
	CancelButton->setText("Cancel");
	connect(CancelButton, &QPushButton::clicked, this, &EntityDialog::Cancel);
	MainLayout->addWidget(CancelButton, 2, 0);

	OkButton = new QPushButton(this);
	OkButton->setText("Destroy");
	connect(OkButton, &QPushButton::clicked, this, &EntityDialog::Ok);
	MainLayout->addWidget(OkButton, 2, 2);

	// input from user
	exec();

	// apply
	if (!Canceled)
		for (auto i : EntitiesTree->selectedItems())
			DeferredTaskSystem::DestroyEntityImmediate(world, ItemToEntity[i]);
}

//------------------------------------------------------------------------------
Entity* EntityDialog::ReparentEntities(World* world, Dynarray<Entity*> entities, Entity* parent)
{
	setModal(true);
	Canceled = true;
	PredefinedEntities.PushBack(parent);

	// create main layout
	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 1);
	MainLayout->setColumnStretch(1, 1);
	MainLayout->setColumnStretch(2, 1);

	// create potential parents list 

	FirstLabel = new QLabel(this);
	FirstLabel->setText("Select one new parent.");
	MainLayout->addWidget(FirstLabel, 0, 0, 1, 3);

	EntitiesTree = new QTreeWidget(this);
	EntitiesTree->setHeaderLabels(QStringList() << "Name" << "ID");
	MainLayout->addWidget(EntitiesTree, 1, 0, 1, 3);

	for (auto child : world->GetRoot()->GetChildren())
		AddEntity(child);

	QTreeWidget* potentialParents = EntitiesTree;


	SecondLabel = new QLabel(this);
	SecondLabel->setText("Select entities to reparent.");
	MainLayout->addWidget(SecondLabel, 2, 0, 1, 3);

	// create entities list 
	EntitiesTree = new QTreeWidget(this);
	EntitiesTree->setHeaderLabels(QStringList() << "Name" << "ID");
	MainLayout->addWidget(EntitiesTree, 3, 0, 1, 3);
	PredefinedEntities = entities;

	for (auto child : world->GetRoot()->GetChildren())
		AddEntity(child);

	// create and link buttons
	CancelButton = new QPushButton(this);
	CancelButton->setText("Cancel");
	connect(CancelButton, &QPushButton::clicked, this, &EntityDialog::Cancel);
	MainLayout->addWidget(CancelButton, 4, 0);

	OkButton = new QPushButton(this);
	OkButton->setText("Reparent");
	connect(OkButton, &QPushButton::clicked, this, &EntityDialog::Ok);
	MainLayout->addWidget(OkButton, 4, 2);

	// input from user
	exec();

	// apply
	if (!Canceled)
	{
		parent = ItemToEntity[potentialParents->selectedItems()[0]];

		if (!parent)
		{
			QMessageBox msgBox;
			msgBox.setText("Parent can't be null, parenting will be skipped for all entities.");
			msgBox.exec();

			return parent;
		}

		for (auto i : EntitiesTree->selectedItems())
		{
			Entity* child = ItemToEntity[i];

			if (parent == child)
			{
				QMessageBox msgBox;
				msgBox.setText("Child can't be set as its own parent, parenting will be skipped for " +
					i->text(0) + " (" + i->text(1) + ").");
				msgBox.exec();
			}
			else if (child->ContainsChildRecursive(parent))
			{
				QMessageBox msgBox;
				msgBox.setText("Child can't be ancestor of parent, parenting will be skipped for " +
					i->text(0) + " (" + i->text(1) + ").");
				msgBox.exec();
			}
			else
				child->SetParent(parent);
		}
	}

	return parent;
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
