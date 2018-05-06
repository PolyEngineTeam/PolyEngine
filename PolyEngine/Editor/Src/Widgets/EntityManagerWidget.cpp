#include "PolyEditorPCH.hpp"

EntityManagerWidget::EntityManagerWidget()
{
	Layout = new QGridLayout();

	Layout->setColumnStretch(0, 3);
	Layout->setColumnStretch(1, 9);
	Layout->setColumnStretch(2, 2);

	for (int i = 0; i < MAX_COMPONENTS_COUNT + 6; ++i)
		Layout->setRowStretch(i, 1);
	
	setLayout(Layout);

	QPalette disabledEditPalette;
	disabledEditPalette.setColor(QPalette::Base, QColor(218, 218, 218));
	disabledEditPalette.setColor(QPalette::Text, Qt::black);

	// Name
	NameText = new QLabel();
	NameText->setText("Name");
	Layout->addWidget(NameText, 0, 0);

	NameField = new QLineEdit();
	NameField->setText("some name");
	Layout->addWidget(NameField, 0, 1);

	// UniqueID
	UniqueIdText = new QLabel();
	UniqueIdText->setText("UniqueID");
	Layout->addWidget(UniqueIdText, 1, 0);

	UniqueIdField = new QLineEdit();
	UniqueIdField->setText("235646246");
	UniqueIdField->setReadOnly(true);
	UniqueIdField->setPalette(disabledEditPalette);
	Layout->addWidget(UniqueIdField, 1, 1);

	// ParentID / Name
	ParentIdNameText = new QLabel();
	ParentIdNameText->setText("Parent ID / Name");
	Layout->addWidget(ParentIdNameText, 2, 0);

	ParentIdNameField = new QLineEdit();
	ParentIdNameField->setText("235246 / parent name");
	ParentIdNameField->setReadOnly(true);
	ParentIdNameField->setPalette(disabledEditPalette);
	Layout->addWidget(ParentIdNameField, 2, 1);

	ParentIdNameButton = new QPushButton();
	ParentIdNameButton->setText("Select");
	Layout->addWidget(ParentIdNameButton, 2, 2);

	// ChildrenID / Name
	ChildrenIdNameText = new QLabel();
	ChildrenIdNameText->setText("Child ID / Name");
	Layout->addWidget(ChildrenIdNameText, 3, 0);

	ChildrenIdNameField = new QComboBox();
	ChildrenIdNameField->addItem("35 / children1 name");
	ChildrenIdNameField->addItem("235 / children2 name");
	ChildrenIdNameField->addItem("756 / children3 name");
	ChildrenIdNameField->addItem("2 / children4 name");
	Layout->addWidget(ChildrenIdNameField, 3, 1);

	ChildrenIdNameButton = new QPushButton();
	ChildrenIdNameButton->setText("Select");
	Layout->addWidget(ChildrenIdNameButton, 3, 2);

	// Transform
	TransformSection = new SectionContainer("Transform");
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
	
	XLabel = new QLabel();
	XLabel->setText("X");
	XLabel->setAlignment(Qt::AlignCenter);
	transformLayout->addWidget(XLabel, 0, 1);
	
	YLabel = new QLabel();
	YLabel->setText("Y");
	YLabel->setAlignment(Qt::AlignCenter);
	transformLayout->addWidget(YLabel, 0, 2);
	
	ZLabel = new QLabel();
	ZLabel->setText("Z");
	ZLabel->setAlignment(Qt::AlignCenter);
	transformLayout->addWidget(ZLabel, 0, 3);

	for (int y = 0; y < 3; ++y)
		for (int x = 0; x < 3; ++x)
		{
			TranslationField[y][x] = new QLineEdit();
			TranslationField[y][x]->setText("0.0000");
			transformLayout->addWidget(TranslationField[y][x], 1 + y, 1 + x);
		}

	TransformSection->SetContentLayout(transformLayout);
	Layout->addWidget(TransformSection, 4, 0, 1, 3);
}

EntityManagerWidget::~EntityManagerWidget()
{
}

void EntityManagerWidget::SetEntity(::Entity *entity)
{
	Entity = entity;
	UpdateWidget();
}

void EntityManagerWidget::UpdateWidget()
{
	// general data
	UniqueIdField->setText(QString(String::From((int)Entity->GetID().GetHash()).GetCStr()));
	ParentIdNameField->setText(QString(String::From((int)Entity->GetParent()->GetID().GetHash()).GetCStr()));
	ChildrenIdNameField->clear();
	for (auto child : Entity->GetChildren())
		ChildrenIdNameField->addItem(QString(String::From((int)child->GetID().GetHash()).GetCStr()));
	
	// transform
	Vector translation = Entity->GetTransform().GetLocalTranslation();
	TranslationField[0][0]->setText(QString(String::From(translation.X).GetCStr()));
	TranslationField[0][1]->setText(QString(String::From(translation.Y).GetCStr()));
	TranslationField[0][2]->setText(QString(String::From(translation.Z).GetCStr()));
	
	EulerAngles rotation = Entity->GetTransform().GetLocalRotation().ToEulerAngles();
	TranslationField[1][0]->setText(QString(String::From(rotation.X.AsDegrees()).GetCStr()));
	TranslationField[1][1]->setText(QString(String::From(rotation.Y.AsDegrees()).GetCStr()));
	TranslationField[1][2]->setText(QString(String::From(rotation.Z.AsDegrees()).GetCStr()));
	
	Vector scale = Entity->GetTransform().GetLocalScale();
	TranslationField[2][0]->setText(QString(String::From(scale.X).GetCStr()));
	TranslationField[2][1]->setText(QString(String::From(scale.Y).GetCStr()));
	TranslationField[2][2]->setText(QString(String::From(scale.Z).GetCStr()));

	for (auto cmp : Components)
		Layout->removeWidget(cmp);

	Components.Clear();
	
	// components
	for (int i = 0, row = 5; i < MAX_COMPONENTS_COUNT; ++i)
	{
		ComponentBase* cmp = Entity->GetComponent(i);
		if (!cmp) continue;

		cmp->GetTypeInfo();
		cmp->GetTypeInfo().GetTypeName();

		SectionContainer* section = new SectionContainer(cmp->GetTypeInfo().GetTypeName());
		RTTIViewerWidget* viewer = new RTTIViewerWidget();
		viewer->SetObject(cmp);
		section->SetContentLayout(viewer->layout());
		Components.PushBack(section);

		Layout->addWidget(section, row, 0, 1, 3);
		++row;
	}
}
