#pragma once

#include <Core.hpp>
#include <ECS/World.hpp>

#include <qmenu.h>
#include <QtWidgets/qgridlayout.h>

#include "Widgets/Inspectors/InspectorWidgetBase.hpp"

class SectionContainer;
class RTTIInspectorWidget;
class TransformControl;

using namespace Poly;

// Shows list of world components in current world.
class WorldComponentsInspectorWidget : public InspectorWidgetBase
{
public:
	WorldComponentsInspectorWidget(QWidget* parent);

	// Initializes object connections with other inspectors and inspector manager.
	void InitializeConnections() override;

	// Removes all component from layout.
	void Reset() override;

	// Set entity to display; updates automatically.
	void SetObject(World* world);

	// Updates every field from every component.
	void UpdateInspector();

	// Destroys and creates everything again.
	void ReloadInspector();

private:
	World* World;

	QGridLayout* MainLayout;

	QMenu* ContextMenu;
		QAction* AddComponentAction;
		QAction* RemoveComponentAction;

	Dynarray<SectionContainer*> ComponentSections;
	Dynarray<RTTIInspectorWidget*> ComponentInspectors;

public slots:
	void SpawnContextMenu(QPoint pos);
		void AddComponent();
		void RemoveComponent();
};