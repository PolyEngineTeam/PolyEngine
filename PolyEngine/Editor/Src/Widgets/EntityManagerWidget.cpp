#include "PolyEditorPCH.hpp"

EntityManagerWidget::EntityManagerWidget()
{
	MainLayout = std::make_unique<QGridLayout>();

	ScrollArea = std::make_unique<QScrollArea>();
	ScrollArea->setLayout(MainLayout.get());

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
	NameText = std::make_unique<QLabel>();
	NameText->setText("Name");
	MainLayout->addWidget(NameText.get(), 0, 0);

	NameField = std::make_unique<QLineEdit>();
	MainLayout->addWidget(NameField.get(), 0, 1);

	// UniqueID
	UniqueIdText = std::make_unique<QLabel>();
	UniqueIdText->setText("UniqueID");
	MainLayout->addWidget(UniqueIdText.get(), 1, 0);

	UniqueIdField = std::make_unique<QLineEdit>();
	UniqueIdField->setReadOnly(true);
	UniqueIdField->setPalette(disabledEditPalette);
	MainLayout->addWidget(UniqueIdField.get(), 1, 1);

	// ParentID / Name
	ParentIdNameText = std::make_unique<QLabel>();
	ParentIdNameText->setText("Parent ID / Name");
	MainLayout->addWidget(ParentIdNameText.get(), 2, 0);

	ParentIdNameField = std::make_unique<QLineEdit>();
	ParentIdNameField->setReadOnly(true);
	ParentIdNameField->setPalette(disabledEditPalette);
	MainLayout->addWidget(ParentIdNameField.get(), 2, 1);

	ParentIdNameButton = std::make_unique<QPushButton>();
	ParentIdNameButton->setText("Select");
	MainLayout->addWidget(ParentIdNameButton.get(), 2, 2);

	// ChildrenID / Name
	ChildrenIdNameText = std::make_unique<QLabel>();
	ChildrenIdNameText->setText("Child ID / Name");
	MainLayout->addWidget(ChildrenIdNameText.get(), 3, 0);

	ChildrenIdNameField = std::make_unique<QComboBox>();
	MainLayout->addWidget(ChildrenIdNameField.get(), 3, 1);

	ChildrenIdNameButton = std::make_unique<QPushButton>();
	ChildrenIdNameButton->setText("Select");
	MainLayout->addWidget(ChildrenIdNameButton.get(), 3, 2);

	// Transform
	TransformSection = std::make_unique<SectionContainer>("Transform");
	QGridLayout* transformLayout = new QGridLayout();

	transformLayout->setColumnStretch(0, 1);
	transformLayout->setColumnStretch(1, 1);
	transformLayout->setColumnStretch(2, 1);
	transformLayout->setColumnStretch(3, 1);

	TranslationLabel = std::make_unique<QLabel>();
	TranslationLabel->setText("Translation");
	transformLayout->addWidget(TranslationLabel.get(), 1, 0);
	
	RotationLabel = std::make_unique<QLabel>();
	RotationLabel->setText("Rotation");
	transformLayout->addWidget(RotationLabel.get(), 2, 0);
	
	ScaleLabel = std::make_unique<QLabel>();
	ScaleLabel->setText("Scale");
	transformLayout->addWidget(ScaleLabel.get(), 3, 0);
	
	XLabel = std::make_unique<QLabel>();
	XLabel->setText("X");
	XLabel->setAlignment(Qt::AlignCenter);
	transformLayout->addWidget(XLabel.get(), 0, 1);
	
	YLabel = std::make_unique<QLabel>();
	YLabel->setText("Y");
	YLabel->setAlignment(Qt::AlignCenter);
	transformLayout->addWidget(YLabel.get(), 0, 2);
	
	ZLabel = std::make_unique<QLabel>();
	ZLabel->setText("Z");
	ZLabel->setAlignment(Qt::AlignCenter);
	transformLayout->addWidget(ZLabel.get(), 0, 3);

	for (int y = 0; y < 3; ++y)
		for (int x = 0; x < 3; ++x)
		{
			TranslationField[y][x] = std::make_unique<QLineEdit>();
			transformLayout->addWidget(TranslationField[y][x].get(), 1 + y, 1 + x);
		}

	TransformSection->SetLayout(transformLayout);
	MainLayout->addWidget(TransformSection.get(), 4, 0, 1, 3);

	setLayout(MainLayout.get());
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

	// remove all old entity component sections
	for (auto cmp : ComponentWidgets)
		delete cmp;
	ComponentWidgets.Clear();

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

		section->SetLayout(viewer->layout());
		MainLayout->addWidget(section, row, 0, 1, 3);
		++row;
	}
}
