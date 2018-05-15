#include "PolyEditorPCH.hpp"

EntityInspectorWidget::EntityInspectorWidget()
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &EntityInspectorWidget::customContextMenuRequested, this, &EntityInspectorWidget::SpawnContextMenu);

	MainLayout = new QGridLayout(this);
	ContextMenu = new QMenu(this);
	ContextMenu->addAction(new QAction("Add Component", this));
	ContextMenu->addAction(new QAction("Remove Component", this));
	
	MainLayout->setColumnStretch(0, 3);
	MainLayout->setColumnStretch(1, 9);
	MainLayout->setColumnStretch(2, 2);
	
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
	QGridLayout* transformLayout = new QGridLayout();
	
	transformLayout->setColumnStretch(0, 1);
	transformLayout->setColumnStretch(1, 1);
	transformLayout->setColumnStretch(2, 1);
	transformLayout->setColumnStretch(3, 1);
	
	TranslationLabel = new QLabel();
	TranslationLabel->setText("Translation");
	transformLayout->addWidget(TranslationLabel, 1, 0);
	
	RotationLabel = new QLabel();
	RotationLabel->setText("Rotation");
	transformLayout->addWidget(RotationLabel, 2, 0);
	
	ScaleLabel = new QLabel();
	ScaleLabel->setText("Scale");
	transformLayout->addWidget(ScaleLabel, 3, 0);
	
	XYZLabels[0] = new QLabel();
	XYZLabels[0]->setText("X");
	XYZLabels[0]->setAlignment(Qt::AlignCenter);
	transformLayout->addWidget(XYZLabels[0], 0, 1);
	
	XYZLabels[1] = new QLabel();
	XYZLabels[1]->setText("Y");
	XYZLabels[1]->setAlignment(Qt::AlignCenter);
	transformLayout->addWidget(XYZLabels[1], 0, 2);
	
	XYZLabels[2] = new QLabel();
	XYZLabels[2]->setText("Z");
	XYZLabels[2]->setAlignment(Qt::AlignCenter);
	transformLayout->addWidget(XYZLabels[2], 0, 3);
	
	for (int x = 0; x < 3; ++x)
	{
		TranslationField[x] = new QLineEdit();
		transformLayout->addWidget(TranslationField[x], 1, 1 + x);
	
		RotationField[x] = new QLineEdit();
		transformLayout->addWidget(RotationField[x], 2, 1 + x);
	
		ScaleField[x] = new QLineEdit();
		transformLayout->addWidget(ScaleField[x], 3, 1 + x);
	}
	
	TransformSection->SetLayout(transformLayout);
	MainLayout->addWidget(TransformSection, 4, 0, 1, 3);

	setLayout(MainLayout);
}

void EntityInspectorWidget::SetEntity(::Entity *entity)
{
	Entity = entity;
	UpdateWidget();
}

void EntityInspectorWidget::UpdateEntity()
{
}

void EntityInspectorWidget::UpdateWidget()
{
	// general data
	UniqueIdField->setText(QString(String::From((int)Entity->GetID().GetHash()).GetCStr()));
	ParentIdNameField->setText(QString(String::From((int)Entity->GetParent()->GetID().GetHash()).GetCStr()));
	ChildrenIdNameField->clear();
	for (auto child : Entity->GetChildren())
		ChildrenIdNameField->addItem(QString(String::From((int)child->GetID().GetHash()).GetCStr()));
	
	// transform
	Vector translation = Entity->GetTransform().GetLocalTranslation();
	TranslationField[0]->setText(QString(String::From(translation.X).GetCStr()));
	TranslationField[1]->setText(QString(String::From(translation.Y).GetCStr()));
	TranslationField[2]->setText(QString(String::From(translation.Z).GetCStr()));
	
	EulerAngles rotation = Entity->GetTransform().GetLocalRotation().ToEulerAngles();
	RotationField[0]->setText(QString(String::From(rotation.X.AsDegrees()).GetCStr()));
	RotationField[1]->setText(QString(String::From(rotation.Y.AsDegrees()).GetCStr()));
	RotationField[2]->setText(QString(String::From(rotation.Z.AsDegrees()).GetCStr()));
	
	Vector scale = Entity->GetTransform().GetLocalScale();
	ScaleField[0]->setText(QString(String::From(scale.X).GetCStr()));
	ScaleField[1]->setText(QString(String::From(scale.Y).GetCStr()));
	ScaleField[2]->setText(QString(String::From(scale.Z).GetCStr()));

	// remove all old entity component sections
	for (auto cmp : ComponentSections)
	{
		MainLayout->removeWidget(cmp);
		delete cmp;
	}
	ComponentSections.Clear();
	
	// add component sections
	for (int i = 0, row = 5; i < MAX_COMPONENTS_COUNT; ++i)
	{
		ComponentBase* cmp = Entity->GetComponent(i);
		if (!cmp) continue;

		cmp->GetTypeInfo();
		cmp->GetTypeInfo().GetTypeName();

		SectionContainer* section = new SectionContainer(cmp->GetTypeInfo().GetTypeName());
		RTTIViewerWidget* viewer = new RTTIViewerWidget();

		viewer->SetObject(cmp);

		ComponentSections.PushBack(section);
		ComponentWidgets.PushBack(viewer);

		section->SetWidget(viewer);
		MainLayout->addWidget(section, row, 0, 1, 3);
		++row;
	}
}

void EntityInspectorWidget::SpawnContextMenu(QPoint pos)
{
	ContextMenu->popup(this->mapToGlobal(pos));
}

void EntityInspectorWidget::AddComponent()
{
}

void EntityInspectorWidget::RemoveComponent()
{
}