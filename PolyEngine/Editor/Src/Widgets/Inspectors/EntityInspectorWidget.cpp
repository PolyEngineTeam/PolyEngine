#include "PolyEditorPCH.hpp"

#include <sstream>

//------------------------------------------------------------------------------
EntityInspectorWidget::EntityInspectorWidget(QWidget* parent)
	: InspectorWidgetBase(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &EntityInspectorWidget::customContextMenuRequested, this, &EntityInspectorWidget::SpawnContextMenu);

	// main layput
	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 5);
	MainLayout->setColumnStretch(1, 15);
	MainLayout->setColumnStretch(2, 3);
	MainLayout->setColumnStretch(3, 3);

	// context menu
	ContextMenu = new QMenu(this);
		
		AddComponentAction = new QAction("Add Component", this);
		ContextMenu->addAction(AddComponentAction);
		connect(AddComponentAction, &QAction::triggered, this, &EntityInspectorWidget::AddComponent);

		RemoveComponentAction = new QAction("Remove Component", this);
		ContextMenu->addAction(RemoveComponentAction);
		connect(RemoveComponentAction, &QAction::triggered, this, &EntityInspectorWidget::RemoveComponent);
	
	// TODO(squares): find better way
	for (int i = 0; i < MAX_COMPONENTS_COUNT + 6; ++i)
		MainLayout->setRowStretch(i, 1);
	
	
	QPalette disabledEditPalette;
	disabledEditPalette.setColor(QPalette::Base, QColor(218, 218, 218));
	disabledEditPalette.setColor(QPalette::Text, Qt::black);
	
	// Name
	NameText = new QLabel(this);
	NameText->setText("Name");
	MainLayout->addWidget(NameText, 0, 0);
	
	NameField = new StringControl(this);
	MainLayout->addWidget(NameField, 0, 1);
	
	// UniqueID
	UniqueIdText = new QLabel(this);
	UniqueIdText->setText("UniqueID");
	MainLayout->addWidget(UniqueIdText, 1, 0);
	
	UniqueIdField = new QLineEdit(this);
	UniqueIdField->setReadOnly(true);
	UniqueIdField->setPalette(disabledEditPalette);
	MainLayout->addWidget(UniqueIdField, 1, 1);
	
	// ParentID / Name
	ParentIdNameText = new QLabel(this);
	ParentIdNameText->setText("Name / Parent ID");
	MainLayout->addWidget(ParentIdNameText, 2, 0);
	
	ParentIdNameField = new QLineEdit(this);
	ParentIdNameField->setReadOnly(true);
	ParentIdNameField->setPalette(disabledEditPalette);
	MainLayout->addWidget(ParentIdNameField, 2, 1);
	
	ParentSelectButton = new QPushButton(this);
	ParentSelectButton->setText("Select");
	MainLayout->addWidget(ParentSelectButton, 2, 2);
	connect(ParentSelectButton, &QPushButton::clicked, this, &EntityInspectorWidget::SelectParent);

	ParentChangeButton = new QPushButton(this);
	ParentChangeButton->setText("Change");
	MainLayout->addWidget(ParentChangeButton, 2, 3);
	connect(ParentChangeButton, &QPushButton::clicked, this, &EntityInspectorWidget::ChangeParent);

	// ChildrenID / Name
	ChildrenIdNameText = new QLabel(this);
	ChildrenIdNameText->setText("Name / Child ID");
	MainLayout->addWidget(ChildrenIdNameText, 3, 0);
	
	ChildrenIdNameField = new QComboBox(this);
	MainLayout->addWidget(ChildrenIdNameField, 3, 1);
	
	ChildrenSelectButton = new QPushButton(this);
	ChildrenSelectButton->setText("Select");
	MainLayout->addWidget(ChildrenSelectButton, 3, 2);
	connect(ChildrenSelectButton, &QPushButton::clicked, this, &EntityInspectorWidget::SelectChild);

	// Transform
	TransformSection = new SectionContainer("Transform", this);
	Transform = new TransformControl(this);
	TransformSection->SetWidget(Transform);

	MainLayout->addWidget(TransformSection, 4, 0, 1, 3);

	ReloadInspector();
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::InitializeConnections()
{
	connect(gApp->InspectorMgr, &InspectorManager::EntitiesSelectionChanged,
		this, &EntityInspectorWidget::SetSelectedEntities);
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::Reset()
{
	Entities.Clear();

	NameField->Reset();
	UniqueIdField->setText("");
	ParentIdNameField->setText("");
	ChildrenIdNameField->clear();

	// remove all old entity component sections
	for (auto cmp : ComponentSections)
	{
		MainLayout->removeWidget(cmp);
		delete cmp;
	}
	ComponentSections.Clear();
	ComponentInspectors.Clear();
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::UpdateInspector()
{
	if (Entities.GetSize() == 1)
	{
		std::stringstream ss;

		NameField->UpdateControl();

		ss << Entities[0]->GetID();
		UniqueIdField->setText(&ss.str()[0]);

		ss.str(std::string());
		ss << Entities[0]->GetParent()->GetID();
		ParentIdNameField->setText(&ss.str()[0]);
	}
	else if (Entities.GetSize() > 1)
	{
		bool sameParent = true;
		Entity* parent = Entities[0]->GetParent();

		for (Entity* e : Entities)
			if (e->GetParent() != parent)
			{
				sameParent = false;
				break;
			}

		if (sameParent)
		{
			std::stringstream ss;
			ss.str(std::string());
			ss << Entities[0]->GetParent()->GetID();
			ParentIdNameField->setText(&ss.str()[0]);
		}
		else
		{
			ParentIdNameField->setText("< multiple selection >");
		}
	}
	else
		throw new std::exception("Unsupported case.");

	for (RTTIInspectorWidget* inspector : ComponentInspectors)
		inspector->UpdateInspector();
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::ReloadInspector()
{
	// remove all old entity component sections
	for (auto cmp : ComponentSections)
	{
		MainLayout->removeWidget(cmp);
		delete cmp;
	}
	ComponentSections.Clear();
	ComponentInspectors.Clear();

	// no entity selected
	if (Entities.GetSize() == 0)
	{
		NameField->Reset();

		UniqueIdField->setText("");

		ParentIdNameField->setText("");
		ParentChangeButton->hide();
		ParentSelectButton->hide();

		ChildrenIdNameField->clear();
		ChildrenSelectButton->hide();

		Transform->Reset();
		TransformSection->hide();
	}
	else if (Entities.GetSize() > 1)
	{
		NameField->SetText("< multiple selection >");

		UniqueIdField->setText("< multiple selection >");


		bool sameParent = true;
		Entity* parent = Entities[0]->GetParent();

		for (Entity* e : Entities)
			if (e->GetParent() != parent)
			{
				sameParent = false;
				break;
			}


		if (sameParent)
		{
			std::stringstream ss;
			ss.str(std::string());
			ss << Entities[0]->GetParent()->GetID();
			ParentIdNameField->setText(&ss.str()[0]);
			ParentChangeButton->show();
			ParentSelectButton->show();
		}
		else
		{
			ParentIdNameField->setText("< multiple selection >");
			ParentChangeButton->show();
			ParentSelectButton->hide();
		}

		ChildrenIdNameField->clear();
		ChildrenSelectButton->hide();

		Transform->Reset();
		TransformSection->hide();
	}
	else
	{
		// general data
		std::stringstream ss;

		NameField->SetObject(&Entities[0]->Name, &Entities[0]->GetPropertyManager()->GetPropertyList()[1]);

		ss << Entities[0]->GetID();
		UniqueIdField->setText(&ss.str()[0]);

		ss.str(std::string());
		ss << Entities[0]->GetParent()->GetID();
		ParentIdNameField->setText(&ss.str()[0]);
		ParentChangeButton->show();
		ParentSelectButton->show();

		ChildrenIdNameField->clear();
		for (auto child : Entities[0]->GetChildren())
		{
			ss.str(std::string());
			ss << child->GetID();
			ChildrenIdNameField->addItem(&ss.str()[0]);
		}
		ChildrenSelectButton->show();

		TransformSection->show();
		Transform->SetObject(Entities[0], &Entities[0]->GetPropertyManager()->GetPropertyList()[0]);

		// add component sections
		for (int i = 0, row = 5; i < MAX_COMPONENTS_COUNT; ++i)
		{
			ComponentBase* cmp = Entities[0]->GetComponent(i);
			if (!cmp) continue;

			cmp->GetTypeInfo();
			cmp->GetTypeInfo().GetTypeName();

			SectionContainer* section = new SectionContainer(cmp->GetTypeInfo().GetTypeName());
			RTTIInspectorWidget* viewer = new RTTIInspectorWidget(this);

			viewer->SetObject(cmp);

			ComponentSections.PushBack(section);
			ComponentInspectors.PushBack(viewer);

			section->SetWidget(viewer);
			MainLayout->addWidget(section, row, 0, 1, 3);
			++row;
		}
	}
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::SetSelectedEntities(Dynarray<Entity*> entities)
{
	Entities = entities;
	ReloadInspector();
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::SpawnContextMenu(QPoint pos)
{
	ContextMenu->popup(this->mapToGlobal(pos));
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::AddComponent()
{
	AddComponentDialog dialog(Entities[0]);
	dialog.exec();

	if (!dialog.OperationCanceled())
		ReloadInspector();
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::RemoveComponent()
{
	RemoveComponentDialog dialog(Entities[0]);
	dialog.exec();

	if (!dialog.OperationCanceled())
		ReloadInspector();
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::ChangeParent()
{
	ChangeParentDialog dialog(Entities);
	dialog.exec();

	if (!dialog.OperationCanceled())
		UpdateInspector();
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::SelectParent()
{
	emit gApp->InspectorMgr->EntitiesSelectionChanged({ Entities[0]->GetParent() });
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::SelectChild()
{
	emit gApp->InspectorMgr->EntitiesSelectionChanged(
		{ Entities[0]->GetChildren()[ChildrenIdNameField->currentIndex()] });
}
