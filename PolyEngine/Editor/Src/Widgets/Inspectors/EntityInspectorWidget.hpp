#pragma once

#include <ECS/Entity.hpp>
#include <Core.hpp>

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <qscrollarea.h>

#include "Widgets/PolyWidget.hpp"

class SectionContainer;
class TransformControl;
class RTTIInspectorWidget;

using namespace Poly;

// This widget allows user change entity and its components properties.
// User also can add and remove any component.
class EntityInspectorWidget : public PolyWidget
{
public:
	EntityInspectorWidget();

	// Set entity to display; updates automatically.
	void SetObject(Entity* entity);

	// Updates every property of entity and its components from widget fields.
	void UpdateObject();

	// Updates every field within widget from currently set entity.
	void UpdateInspector();

private:
	Entity* Entity;

	QGridLayout* MainLayout;
	QMenu* ContextMenu;
		QAction* AddComponentAction;
		QAction* RemoveComponentAction;

	QLabel* NameText;			QLineEdit* NameField;
	QLabel* UniqueIdText;		QLineEdit* UniqueIdField;
	QLabel* ParentIdNameText;	QLineEdit* ParentIdNameField;	QPushButton* ParentIdNameButton;
	QLabel* ChildrenIdNameText;	QComboBox* ChildrenIdNameField;	QPushButton* ChildrenIdNameButton;

	SectionContainer* TransformSection;
	TransformControl* Transform;

	// Component sections
	Dynarray<SectionContainer*> ComponentSections;
	Dynarray<RTTIInspectorWidget*> ComponentInspectors;

public slots:
	void SpawnContextMenu(QPoint pos); 
	void AddComponent(); 
	void RemoveComponent();
};