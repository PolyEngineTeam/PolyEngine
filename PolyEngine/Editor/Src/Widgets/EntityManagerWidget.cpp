#include "PolyEditorPCH.hpp"

const int BEGIN_Y = 20;
const int DELTA_Y = 18 + 6;
const int COMPONENTS_OFFSET = 10 + 10 * DELTA_Y;

EntityManagerWidget::EntityManagerWidget(const QString& title, QWidget* parent) :
	PolyWidget(title, parent)
{
	setFixedWidth(420);

	QPalette *disabledEditPalette = new QPalette();
	disabledEditPalette->setColor(QPalette::Base, QColor(218, 218, 218));
	disabledEditPalette->setColor(QPalette::Text, Qt::black);

	GeneralDataGroup = new QGroupBox(this);
	GeneralDataGroup->setTitle("General Data");
	GeneralDataGroup->move(12, 6);
	GeneralDataGroup->setFixedWidth(400);

	// UniqueID
	UniqueIdText = new QLabel(GeneralDataGroup);
	UniqueIdText->move(12, BEGIN_Y);
	UniqueIdText->resize(100, 18);
	UniqueIdText->setText("UniqueID");

	UniqueIdField = new QLineEdit(GeneralDataGroup);
	UniqueIdField->move(124, BEGIN_Y);
	UniqueIdField->resize(200, 18);
	UniqueIdField->setText("2");
	UniqueIdField->setReadOnly(GeneralDataGroup);
	UniqueIdField->setPalette(*disabledEditPalette);

	// ParentID
	ParentIdText = new QLabel(GeneralDataGroup);
	ParentIdText->move(12, BEGIN_Y + 1 * DELTA_Y);
	ParentIdText->resize(100, 18);
	ParentIdText->setText("Parent ID");

	ParentIdField = new QLineEdit(GeneralDataGroup);
	ParentIdField->move(124, BEGIN_Y + 1 * DELTA_Y);
	ParentIdField->resize(200, 18);
	ParentIdField->setText("1");
	ParentIdField->setReadOnly(true);
	ParentIdField->setPalette(*disabledEditPalette);

	ParentIdButton = new QPushButton(GeneralDataGroup);
	ParentIdButton->move(336, BEGIN_Y + 1 * DELTA_Y);
	ParentIdButton->resize(50, 18);
	ParentIdButton->setText("Select");
	//QObject::connect(ParentIdButton, &QPushButton::clicked, this, &CreateProjectDialog::BrowseProjectDirectory);

	// ChildrenID
	ChildrenIdText = new QLabel(GeneralDataGroup);
	ChildrenIdText->move(12, BEGIN_Y + 2 * DELTA_Y);
	ChildrenIdText->resize(100, 18);
	ChildrenIdText->setText("Parent ID");

	ChildrenIdField = new QComboBox(GeneralDataGroup);
	ChildrenIdField->move(124, BEGIN_Y + 2 * DELTA_Y);
	ChildrenIdField->resize(200, 18);
	ChildrenIdField->addItem("3");
	ChildrenIdField->addItem("4");
	ChildrenIdField->addItem("5");
	ChildrenIdField->addItem("6");
	ChildrenIdField->addItem("7");
	ChildrenIdField->setPalette(*disabledEditPalette);

	ChildrenIdButton = new QPushButton(GeneralDataGroup);
	ChildrenIdButton->move(336, BEGIN_Y + 2 * DELTA_Y);
	ChildrenIdButton->resize(50, 18);
	ChildrenIdButton->setText("Select");
	//QObject::connect(ParentIdButton, &QPushButton::clicked, this, &CreateProjectDialog::BrowseProjectDirectory);

	// translation
	TransformGroup = new QGroupBox(this);
	TransformGroup->setTitle("Local Transform");
	TransformGroup->move(12, BEGIN_Y + 4 * DELTA_Y);
	TransformGroup->setFixedWidth(400);

	TranslationLabel = new QLabel(TransformGroup);
	TranslationLabel->move(12, BEGIN_Y + 1 * DELTA_Y);
	TranslationLabel->resize(100, 18);
	TranslationLabel->setText("Translation");

	RotationLabel = new QLabel(TransformGroup);
	RotationLabel->move(12, BEGIN_Y + 2 * DELTA_Y);
	RotationLabel->resize(100, 18);
	RotationLabel->setText("Rotation");

	ScaleLabel = new QLabel(TransformGroup);
	ScaleLabel->move(12, BEGIN_Y + 3 * DELTA_Y);
	ScaleLabel->resize(100, 18);
	ScaleLabel->setText("Scale");

	XLabel = new QLabel(TransformGroup);
	XLabel->move(124, BEGIN_Y);
	XLabel->resize(50, 18);
	XLabel->setText("X");

	XLabel = new QLabel(TransformGroup);
	XLabel->move(213, BEGIN_Y);
	XLabel->resize(50, 18);
	XLabel->setText("Y");

	XLabel = new QLabel(TransformGroup);
	XLabel->move(302, BEGIN_Y);
	XLabel->resize(50, 18);
	XLabel->setText("Z");

	for (int a = 0; a < 3; ++a)
		for (int b = 0; b < 3; ++b)
		{
			TranslationField[a][b] = new QLineEdit(TransformGroup);
			TranslationField[a][b]->move(124 + 89 * b, BEGIN_Y + (1 + a) * DELTA_Y);
			TranslationField[a][b]->resize(85, 18);
			TranslationField[a][b]->setText("0,0000");
			TranslationField[a][b]->setValidator(new QDoubleValidator(-10000, 10000, 6, TranslationField[a][b]));
		}

	ComponentGroup = new QGroupBox(this);
	ComponentGroup->setTitle("Components");
	ComponentGroup->move(12, COMPONENTS_OFFSET);
	ComponentGroup->setFixedWidth(400);

	ChildrenIdText = new QLabel(ComponentGroup);
	ChildrenIdText->move(12, BEGIN_Y + 0 * DELTA_Y);
	ChildrenIdText->resize(100, 18);
	ChildrenIdText->setText("Component");

	ComponentField = new QComboBox(ComponentGroup);
	ComponentField->move(124, BEGIN_Y + 0 * DELTA_Y);
	ComponentField->resize(200, 18);
	ComponentField->addItem("MeshComponent");
	ComponentField->addItem("PathfindingComponent");
	ComponentField->addItem("SoundEmitterComponent");
	ComponentField->addItem("Collider3DComponent");
	ComponentField->addItem("Rigidbody3DComponent");
	ComponentField->setPalette(*disabledEditPalette);
}

EntityManagerWidget::~EntityManagerWidget()
{
}

void EntityManagerWidget::SetEntity(::Entity *entity)
{
	Entity = entity;
	Update();
}

void EntityManagerWidget::Update()
{
	// general data
	UniqueIdField->setText(QString(String::From((int)Entity->GetID().GetHash()).GetCStr()));
	ParentIdField->setText(QString(String::From((int)Entity->GetParent()->GetID().GetHash()).GetCStr()));
	for (auto child : Entity->GetChildren())
		ChildrenIdField->addItem(QString(String::From((int)child->GetID().GetHash()).GetCStr()));

	// transform
	Vector translation = Entity->GetTransform().GetLocalTranslation();
	TranslationField[0][0]->setText(QString(String::From(translation.X).GetCStr()));
	TranslationField[1][0]->setText(QString(String::From(translation.Y).GetCStr()));
	TranslationField[2][0]->setText(QString(String::From(translation.Z).GetCStr()));

	EulerAngles rotation = Entity->GetTransform().GetLocalRotation().ToEulerAngles();
	TranslationField[0][0]->setText(QString(String::From(rotation.X.AsDegrees()).GetCStr()));
	TranslationField[1][0]->setText(QString(String::From(rotation.Y.AsDegrees()).GetCStr()));
	TranslationField[2][0]->setText(QString(String::From(rotation.Z.AsDegrees()).GetCStr()));

	Vector scale = Entity->GetTransform().GetLocalScale();
	TranslationField[0][0]->setText(QString(String::From(scale.X).GetCStr()));
	TranslationField[1][0]->setText(QString(String::From(scale.Y).GetCStr()));
	TranslationField[2][0]->setText(QString(String::From(scale.Z).GetCStr()));

	// components
	//for (auto component : Entity->GetWorld()->)
}
