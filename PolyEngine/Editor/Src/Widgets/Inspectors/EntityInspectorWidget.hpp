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
class RTTIViewerWidget;

using namespace Poly;

// This widget allows user change entity and its components properties.
// User also can add and remove any component.
class EntityInspectorWidget : public PolyWidget
{
public:
	EntityInspectorWidget();

	// Set entity to display; updates automatically.
	void SetEntity(Entity* entity);

	// Updates every property of entity and its components from widget fields.
	void UpdateEntity();

	// Updates every field within widget from currently set entity.
	void UpdateWidget();

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
								QLabel* XYZLabels[3];
	QLabel* TranslationLabel;	QLineEdit* TranslationField[3];	
	QLabel* RotationLabel;		QLineEdit* RotationField[3];
	QLabel* ScaleLabel;			QLineEdit* ScaleField[3];

	// Component sections
	Dynarray<SectionContainer*> ComponentSections;
	Dynarray<RTTIViewerWidget*> ComponentWidgets;

public slots:
	void SpawnContextMenu(QPoint pos); 
	void AddComponent(); 
	void RemoveComponent();
};