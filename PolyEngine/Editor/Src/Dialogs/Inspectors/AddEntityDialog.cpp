#include "PolyEditorPCH.hpp"

AddEntityDialog::AddEntityDialog(::World* world, Entity* parent)
	: World(world), Parent(parent)
{
	setModal(true);
	setMaximumHeight(800);

	// create main layout
	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 1);
	MainLayout->setColumnStretch(1, 1);
	MainLayout->setColumnStretch(2, 1);

	// create list
	List = new QListWidget(this);
	MainLayout->addWidget(List, 0, 0, 1, 3);
	List->addItem(new QListWidgetItem(tr("Empty entity"), List));
	List->addItem(new QListWidgetItem(tr("SpikesLeft"), List));
	List->addItem(new QListWidgetItem(tr("SpikesRight"), List));
	List->addItem(new QListWidgetItem(tr("SpikesTop"), List));
	List->addItem(new QListWidgetItem(tr("SpikesBottom"), List));
	List->addItem(new QListWidgetItem(tr("Tile"), List));
	List->addItem(new QListWidgetItem(tr("Powerup"), List));

	CancelButton = new QPushButton(this);
	CancelButton->setText("Cancel");
	connect(CancelButton, &QPushButton::clicked, this, &AddEntityDialog::Cancel);
	MainLayout->addWidget(CancelButton, 2, 0);

	OkButton = new QPushButton(this);
	OkButton->setText("Ok");
	connect(OkButton, &QPushButton::clicked, this, &AddEntityDialog::Ok);
	MainLayout->addWidget(OkButton, 2, 2);
}

void AddEntityDialog::Ok()
{
	// apply changes
	if (List->selectedItems()[0] == List->item(0))
	{
		Result = DeferredTaskSystem::SpawnEntityImmediate(World);
		
		if (Parent)
			Result->SetParent(Parent);
	}

	close();
}

void AddEntityDialog::Cancel()
{
	Canceled = true;

	close();
}
