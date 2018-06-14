#include "PolyEditorPCH.hpp"

#include <sstream>

ChangeParentDialog::ChangeParentDialog(Dynarray<Entity*> entities, Entity* parent)
{
	if (parent)
	{
		for (Entity* e : entities)
			e->SetParent(parent);

		close();
		return;
	}

	setModal(true);
	setMaximumHeight(800);

	Entities = entities;

	// create main layout
	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 1);
	MainLayout->setColumnStretch(1, 1);
	MainLayout->setColumnStretch(2, 1);

	// create group box
	Tree = new QTreeWidget(this);
	Tree->setHeaderLabels(QStringList() << "Entity Name" << "Entity ID");

	for (auto child : entities[0]->GetWorld()->GetRoot()->GetChildren())
		AddEntity(child);

	MainLayout->addWidget(Tree, 0, 0, 1, 3);

	QPalette disabledEditPalette;
	disabledEditPalette.setColor(QPalette::Base, QColor(218, 218, 218));
	disabledEditPalette.setColor(QPalette::Text, Qt::black);

	EntityIdNameText = new QLabel(this);
	EntityIdNameText->setText("Entity ID / Name");
	MainLayout->addWidget(EntityIdNameText, 1, 0);

	EntityIdNameField = new QLineEdit(this);
	EntityIdNameField->setReadOnly(true);
	if (entities.GetSize == 1)
	{
		std::stringstream ss;
		ss << entities[0]->GetID();
		EntityIdNameField->setText(&ss.str()[0]);
	}
	else
		EntityIdNameField->setText("< multiple selection >");
	EntityIdNameField->setPalette(disabledEditPalette);
	MainLayout->addWidget(EntityIdNameField, 1, 1, 1, 2);

	CancelButton = new QPushButton(this);
	CancelButton->setText("Cancel");
	connect(CancelButton, &QPushButton::clicked, this, &ChangeParentDialog::Cancel);
	MainLayout->addWidget(CancelButton, 2, 0);

	OkButton = new QPushButton(this);
	OkButton->setText("Ok");
	connect(OkButton, &QPushButton::clicked, this, &ChangeParentDialog::Ok);
	MainLayout->addWidget(OkButton, 2, 2);
}

void ChangeParentDialog::AddEntity(Entity* entity)
{
	QTreeWidgetItem* item = new QTreeWidgetItem(Tree);

	std::stringstream ss;
	ss << entity->GetID();
	item->setText(1, (&ss.str()[0]));
	item->setText(0, entity->Name.GetCStr());

	for (auto child : entity->GetChildren())
		AddEntity(child);
}

void ChangeParentDialog::Ok()
{
	Entity* parent; Tree->selectedItems()[0]->text(1);

	for (Entity* e : Entities)
		e->SetParent(parent);

	close();
}

void ChangeParentDialog::Cancel()
{
	Canceled = true;
	close();
}