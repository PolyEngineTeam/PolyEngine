#include "PolyEditorPCH.hpp"

ComponentDialog::ComponentDialog(::Entity* entity, bool removeComponents)
{
	setModal(true);
	setMaximumHeight(800);

	// create main layout
	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 1);
	MainLayout->setColumnStretch(1, 1);
	MainLayout->setColumnStretch(2, 1);

	// create group box
	Tree = new QTreeWidget(this);
	Tree->setHeaderLabels(QStringList() << (removeComponents ? "Remove" : "Add" ) << "Component Name");

	for (int i = 0; i < MAX_COMPONENTS_COUNT; ++i)
		if ((removeComponents && entity->HasComponent(i))
			|| (!removeComponents && !entity->HasComponent(i)))
		{
			// create component from ID, get its name and delete it

			QTreeWidgetItem* entityTree = new QTreeWidgetItem(Tree);
			entityTree->setText(1, QString::number(i).toLatin1());
			entityTree->setCheckState(0, Qt::Unchecked);
		}

	MainLayout->addWidget(Tree, 0, 0, 1, 3);

	QPalette disabledEditPalette;
	disabledEditPalette.setColor(QPalette::Base, QColor(218, 218, 218));
	disabledEditPalette.setColor(QPalette::Text, Qt::black);

	EntityIdNameText = new QLabel(this);
	EntityIdNameText->setText("Entity ID / Name");
	MainLayout->addWidget(EntityIdNameText, 1, 0);

	EntityIdNameField = new QLineEdit(this);
	EntityIdNameField->setReadOnly(true);
	EntityIdNameField->setText(QString::number(entity->GetID().GetHash()));
	EntityIdNameField->setPalette(disabledEditPalette);
	MainLayout->addWidget(EntityIdNameField, 1, 1, 1, 2);

	CancelButton = new QPushButton(this);
	CancelButton->setText("Cancel");
	connect(CancelButton, &QPushButton::clicked, this, &ComponentDialog::Cancel);
	MainLayout->addWidget(CancelButton, 2, 0);

	OkButton = new QPushButton(this);
	OkButton->setText("Ok");
	connect(OkButton, &QPushButton::clicked, this, &ComponentDialog::Ok);
	MainLayout->addWidget(OkButton, 2, 2);
}

ComponentDialog::ComponentDialog(::World * world, bool removeComponents)
{
	setModal(true);
	setMaximumHeight(800);

	// create main layout
	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 1);
	MainLayout->setColumnStretch(1, 1);
	MainLayout->setColumnStretch(2, 1);

	// create group box
	Tree = new QTreeWidget(this);
	Tree->setHeaderLabels(QStringList() << (removeComponents ? "Remove" : "Add") << "Component Name");

	for (int i = 0; i < MAX_COMPONENTS_COUNT; ++i)
		if ((removeComponents && world->HasWorldComponent(i))
			|| (!removeComponents && !world->HasWorldComponent(i)))
		{
			// create component from ID, get its name and delete it

			QTreeWidgetItem* entityTree = new QTreeWidgetItem(Tree);
			entityTree->setText(1, QString::number(i).toLatin1());
			entityTree->setCheckState(0, Qt::Unchecked);
		}

	MainLayout->addWidget(Tree, 0, 0, 1, 3);

	CancelButton = new QPushButton(this);
	CancelButton->setText("Cancel");
	connect(CancelButton, &QPushButton::clicked, this, &ComponentDialog::Cancel);
	MainLayout->addWidget(CancelButton, 2, 0);

	OkButton = new QPushButton(this);
	OkButton->setText("Ok");
	connect(OkButton, &QPushButton::clicked, this, &ComponentDialog::Ok);
	MainLayout->addWidget(OkButton, 2, 2);
}

void ComponentDialog::Ok()
{
	// apply changes

	close();
}

void ComponentDialog::Cancel()
{
	Canceled = true;

	close();
}
