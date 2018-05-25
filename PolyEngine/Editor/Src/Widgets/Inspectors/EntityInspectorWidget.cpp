#include "PolyEditorPCH.hpp"

#include <sstream>

//------------------------------------------------------------------------------
EntityInspectorWidget::EntityInspectorWidget(QWidget* parent)
	: PolyWidget(parent)
{
	connect(gApp->Ui.WorldExplorer, &WorldInspectorWidget::EntitySelected, this, &EntityInspectorWidget::SetObject);
	connect(gApp->Ui.WorldExplorer, &WorldInspectorWidget::EntityDeselected, this, &EntityInspectorWidget::Reset);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &EntityInspectorWidget::customContextMenuRequested, this, &EntityInspectorWidget::SpawnContextMenu);

	// main layput
	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 3);
	MainLayout->setColumnStretch(1, 9);
	MainLayout->setColumnStretch(2, 2);

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
	
	//NameField = ControlBase::CreateControl(this, RTTI::eCorePropertyType::STRING);
	NameField = new QLineEdit(this);
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
	ParentIdNameText->setText("Parent ID / Name");
	MainLayout->addWidget(ParentIdNameText, 2, 0);
	
	ParentIdNameField = new QLineEdit(this);
	ParentIdNameField->setReadOnly(true);
	ParentIdNameField->setPalette(disabledEditPalette);
	MainLayout->addWidget(ParentIdNameField, 2, 1);
	
	ParentIdNameButton = new QPushButton(this);
	ParentIdNameButton->setText("Select");
	MainLayout->addWidget(ParentIdNameButton, 2, 2);
	
	// ChildrenID / Name
	ChildrenIdNameText = new QLabel(this);
	ChildrenIdNameText->setText("Child ID / Name");
	MainLayout->addWidget(ChildrenIdNameText, 3, 0);
	
	ChildrenIdNameField = new QComboBox(this);
	MainLayout->addWidget(ChildrenIdNameField, 3, 1);
	
	ChildrenIdNameButton = new QPushButton(this);
	ChildrenIdNameButton->setText("Select");
	MainLayout->addWidget(ChildrenIdNameButton, 3, 2);

	// Transform
	TransformSection = new SectionContainer("Transform", this);
	Transform = new TransformControl(this);
	TransformSection->SetWidget(Transform);

	MainLayout->addWidget(TransformSection, 4, 0, 1, 3);
	TransformSection->hide();
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::SetObject(::Entity *entity)
{
	Entity = entity;
	TransformSection->show();
	Transform->SetObject(entity, &entity->GetPropertyManager()->GetPropertyList()[0]);
	UpdateInspector();
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::UpdateObject()
{
	// TODO(squares): set entity name here

	Transform->UpdateControl();

	for (RTTIInspectorWidget* inspector : ComponentInspectors)
		inspector->UpdateObject();
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::UpdateInspector()
{
	std::stringstream ss;

	// general data
	ss << Entity->GetID();
	UniqueIdField->setText(&ss.str()[0]);

	ss.str(std::string());
	ss << Entity->GetParent()->GetID();
	ParentIdNameField->setText(&ss.str()[0]);

	ChildrenIdNameField->clear();
	for (auto child : Entity->GetChildren())
	{
		ss.str(std::string());
		ss << child->GetID();
		ChildrenIdNameField->addItem(&ss.str()[0]);
	}
	
	Transform->UpdateControl();

	// remove all old entity component sections
	for (auto cmp : ComponentSections)
	{
		MainLayout->removeWidget(cmp);
		delete cmp;
	}
	ComponentSections.Clear();
	ComponentInspectors.Clear();
	
	// add component sections
	for (int i = 0, row = 5; i < MAX_COMPONENTS_COUNT; ++i)
	{
		ComponentBase* cmp = Entity->GetComponent(i);
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

//------------------------------------------------------------------------------
void EntityInspectorWidget::SpawnContextMenu(QPoint pos)
{
	ContextMenu->popup(this->mapToGlobal(pos));
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::AddComponent()
{
	ComponentDialog dialog(Entity);
	dialog.exec();
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::RemoveComponent()
{
	ComponentDialog dialog(Entity, true);
	dialog.exec();
}

//------------------------------------------------------------------------------
void EntityInspectorWidget::Reset()
{
	NameField->setText("");
	UniqueIdField->setText("");
	ParentIdNameField->setText("");
	ChildrenIdNameField->clear();

	TransformSection->hide();

	// remove all old entity component sections
	for (auto cmp : ComponentSections)
	{
		MainLayout->removeWidget(cmp);
		delete cmp;
	}
	ComponentSections.Clear();
	ComponentInspectors.Clear();
}
