#include "PolyEditorPCH.hpp"

//------------------------------------------------------------------------------
WorldComponentsInspectorWidget::WorldComponentsInspectorWidget(QWidget* parent)
	: InspectorWidgetBase(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &WorldComponentsInspectorWidget::customContextMenuRequested, this, &WorldComponentsInspectorWidget::SpawnContextMenu);

	// main layput
	MainLayout = new QGridLayout(this);

	// context menu
	ContextMenu = new QMenu(this);
		
		AddComponentAction = new QAction("Add Component", this);
		ContextMenu->addAction(AddComponentAction);
		connect(AddComponentAction, &QAction::triggered, this, &WorldComponentsInspectorWidget::AddComponent);

		RemoveComponentAction = new QAction("Remove Component", this);
		ContextMenu->addAction(RemoveComponentAction);
		connect(RemoveComponentAction, &QAction::triggered, this, &WorldComponentsInspectorWidget::RemoveComponent);
		
	// TODO(squares): find better way
	for (size_t i = 0; i < MAX_COMPONENTS_COUNT + 6; ++i)
		MainLayout->setRowStretch((int)i, 1);
}

//------------------------------------------------------------------------------
void WorldComponentsInspectorWidget::InitializeConnections()
{
	connect(gApp->InspectorMgr, &InspectorManager::EngineInitialized, this, &WorldComponentsInspectorWidget::SetObject);
	connect(gApp->InspectorMgr, &InspectorManager::EngineDeinitialized, this, &WorldComponentsInspectorWidget::Reset);
}

//------------------------------------------------------------------------------
void WorldComponentsInspectorWidget::Reset()
{
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
void WorldComponentsInspectorWidget::SetObject(::World* world)
{
	WorldObj = world;
	ReloadInspector();
}

//------------------------------------------------------------------------------
void WorldComponentsInspectorWidget::UpdateInspector()
{
	for (RTTIInspectorWidget* inspector : ComponentInspectors)
		inspector->UpdateInspector();
}

//------------------------------------------------------------------------------
void WorldComponentsInspectorWidget::ReloadInspector()
{
	// remove all old entity component sections
	for (auto cmp : ComponentSections)
	{
		MainLayout->removeWidget(cmp);
		delete cmp;
	}
	ComponentSections.Clear();
	ComponentInspectors.Clear();

	// add component sections
	for (size_t i = 0; i < MAX_COMPONENTS_COUNT; ++i)
	{
		ComponentBase* cmp = WorldObj->GetWorldComponent(i);
		if (!cmp) continue;

		cmp->GetTypeInfo();
		cmp->GetTypeInfo().GetTypeName();

		SectionContainer* section = new SectionContainer(cmp->GetTypeInfo().GetTypeName());
		RTTIInspectorWidget* viewer = new RTTIInspectorWidget(this);

		viewer->SetObject(cmp);

		ComponentSections.PushBack(section);
		ComponentInspectors.PushBack(viewer);

		section->SetWidget(viewer);
		MainLayout->addWidget(section, (int)i, 0);
	}
}

//------------------------------------------------------------------------------
void WorldComponentsInspectorWidget::SpawnContextMenu(QPoint pos)
{
	ContextMenu->popup(this->mapToGlobal(pos));
}

//------------------------------------------------------------------------------
void WorldComponentsInspectorWidget::AddComponent()
{
}

//------------------------------------------------------------------------------
void WorldComponentsInspectorWidget::RemoveComponent()
{
}
