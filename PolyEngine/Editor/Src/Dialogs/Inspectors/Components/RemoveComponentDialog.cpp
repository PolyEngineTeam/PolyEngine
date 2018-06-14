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

#include "Audio/SoundWorldComponent.hpp"
#include "Debugging/DebugWorldComponent.hpp"
#include "ECS/DeferredTaskWorldComponent.hpp"
#include "Input/InputWorldComponent.hpp"
#include "Physics2D/Physics2DWorldComponent.hpp"
#include "Physics3D/Physics3DWorldComponent.hpp"
#include "Rendering/SkyboxWorldComponent.hpp"
#include "Rendering/ViewportWorldComponent.hpp"
#include "Time/TimeWorldComponent.hpp"

#include "ECS/DeferredTaskSystem.hpp"

#include <sstream>

#define ADD_COMPONENT(COMPONENT) \
	if (entity->HasComponent<COMPONENT>()) \
	{ \
		QTreeWidgetItem* cmp = new QTreeWidgetItem(Tree); \
		cmp->setText(1, #COMPONENT); \
		cmp->setCheckState(0, Qt::Unchecked); \
		ComponentDestroyers.insert(std::pair<QString, ComponentDestroyer>(#COMPONENT, [](::Entity* e) \
		{ Poly::DeferredTaskSystem::RemoveComponent<COMPONENT>(e->GetWorld(), e); })); \
	}

#define ADD_WORLD_COMPONENT(COMPONENT) \
	if (world->HasWorldComponent<COMPONENT>()) \
	{ \
		QTreeWidgetItem* cmp = new QTreeWidgetItem(Tree); \
		cmp->setText(1, #COMPONENT); \
		cmp->setCheckState(0, Qt::Unchecked); \
		WorldComponentDestroyers.insert(std::pair<QString, WorldComponentDestroyer>(#COMPONENT, [](::World* w) \
		{ Poly::DeferredTaskSystem::RemoveWorldComponentImmediate<COMPONENT>(w); })); \
	}

RemoveComponentDialog::RemoveComponentDialog(::Entity* entity)
{
	Entity = entity;

	InitControls();

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

	QPalette disabledEditPalette;
	disabledEditPalette.setColor(QPalette::Base, QColor(218, 218, 218));
	disabledEditPalette.setColor(QPalette::Text, Qt::black);

	EntityIdNameText = new QLabel(this);
	EntityIdNameText->setText("Name / Entity ID");
	MainLayout->addWidget(EntityIdNameText, 1, 0);

	EntityIdNameField = new QLineEdit(this);
	EntityIdNameField->setReadOnly(true);
	std::stringstream ss;
	ss << Entity->GetID();
	EntityIdNameField->setText(QString(Entity->Name.GetCStr()) + QString(&ss.str()[0]));
	EntityIdNameField->setPalette(disabledEditPalette);
	MainLayout->addWidget(EntityIdNameField, 1, 1, 1, 2);
}

RemoveComponentDialog::RemoveComponentDialog(::World* world)
{
	World = world;

	InitControls();

	ADD_WORLD_COMPONENT(SoundWorldComponent)
	ADD_WORLD_COMPONENT(DebugWorldComponent)
	ADD_WORLD_COMPONENT(DeferredTaskWorldComponent)
	ADD_WORLD_COMPONENT(InputWorldComponent)
	ADD_WORLD_COMPONENT(Physics2DWorldComponent)
	ADD_WORLD_COMPONENT(Physics3DWorldComponent)
	ADD_WORLD_COMPONENT(SkyboxWorldComponent)
	ADD_WORLD_COMPONENT(ViewportWorldComponent)
	ADD_WORLD_COMPONENT(TimeWorldComponent)
}

void RemoveComponentDialog::InitControls()
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
	Tree->setHeaderLabels(QStringList() << "Remove" << "Component Name");

	MainLayout->addWidget(Tree, 0, 0, 1, 3);

	CancelButton = new QPushButton(this);
	CancelButton->setText("Cancel");
	connect(CancelButton, &QPushButton::clicked, this, &RemoveComponentDialog::Cancel);
	MainLayout->addWidget(CancelButton, 2, 0);

	OkButton = new QPushButton(this);
	OkButton->setText("Ok");
	connect(OkButton, &QPushButton::clicked, this, &RemoveComponentDialog::Ok);
	MainLayout->addWidget(OkButton, 2, 2);
}

void RemoveComponentDialog::Ok()
{
	if (Entity)
		ComponentDestroyers[Tree->selectedItems()[0]->text(0)](Entity);
	else
		WorldComponentDestroyers[Tree->selectedItems()[0]->text(0)](World);

	close();
}

void RemoveComponentDialog::Cancel()
{
	Canceled = true;

	close();
}
