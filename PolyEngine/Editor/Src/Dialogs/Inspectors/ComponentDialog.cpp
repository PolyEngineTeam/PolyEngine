#include "PolyEditorPCH.hpp"

#include "AI/PathfindingComponent.hpp"
#include "Audio/SoundEmitterComponent.hpp"
#include "Audio/SoundListenerComponent.hpp"
#include "Movement/FreeFloatMovementComponent.hpp"
#include "Physics2D/Physics2DColliders.hpp"
#include "Physics2D/Rigidbody2DComponent.hpp"
#include "Physics3D/Collider3DComponent.hpp"
#include "Physics3D/Rigidbody3DComponent.hpp"
#include "Rendering/Camera/CameraComponent.hpp"
#include "Rendering/Lighting/LightSourceComponent.hpp"
#include "Rendering/Particles/ParticleComponent.hpp"
#include "Rendering/MeshRenderingComponent.hpp"
#include "Rendering/PostprocessSettingsComponent.hpp"
#include "Rendering/SpritesheetComponent.hpp"
#include "UI/ScreenSpaceTextComponent.hpp"

#define ADD_COMPONENT(COMPONENT) \
	if (!entity->HasComponent<COMPONENT>()) \
	{ \
		QTreeWidgetItem* cmp = new QTreeWidgetItem(Tree); \
		cmp->setText(1, #COMPONENT); \
		cmp->setCheckState(0, Qt::Unchecked); \
	}

ComponentDialog::ComponentDialog(::Entity* entity, bool removeComponents)
{
	setModal(true);
	setMaximumHeight(800);

	Entity = entity;

	// create main layout
	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 1);
	MainLayout->setColumnStretch(1, 1);
	MainLayout->setColumnStretch(2, 1);

	// create group box
	Tree = new QTreeWidget(this);
	Tree->setHeaderLabels(QStringList() << (removeComponents ? "Remove" : "Add" ) << "Component Name");

	//for (int i = 0; i < MAX_COMPONENTS_COUNT; ++i)
	//	if ((removeComponents && entity->HasComponent(i))
	//		|| (!removeComponents && !entity->HasComponent(i)))
	//	{
	//		// create component from ID, get its name and delete it
	//
	//		QTreeWidgetItem* entityTree = new QTreeWidgetItem(Tree);
	//		entityTree->setText(1, QString::number(i).toLatin1());
	//		entityTree->setCheckState(0, Qt::Unchecked);
	//	}

	ADD_COMPONENT(PathfindingComponent)
	ADD_COMPONENT(SoundEmitterComponent)
	ADD_COMPONENT(SoundListenerComponent)
	ADD_COMPONENT(FreeFloatMovementComponent)
	ADD_COMPONENT(Box2DColliderComponent)
	ADD_COMPONENT(Circle2DColliderComponent)
	ADD_COMPONENT(RigidBody2DComponent)
	ADD_COMPONENT(Collider3DComponent)
	ADD_COMPONENT(Rigidbody3DComponent)
	ADD_COMPONENT(CameraComponent)
	ADD_COMPONENT(AmbientLightWorldComponent)
	ADD_COMPONENT(DirectionalLightComponent)
	ADD_COMPONENT(PointLightComponent)
	ADD_COMPONENT(SpotLightComponent)
	ADD_COMPONENT(MeshRenderingComponent)
	ADD_COMPONENT(PostprocessSettingsComponent)
	ADD_COMPONENT(SpritesheetComponent)
	ADD_COMPONENT(ScreenSpaceTextComponent)

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