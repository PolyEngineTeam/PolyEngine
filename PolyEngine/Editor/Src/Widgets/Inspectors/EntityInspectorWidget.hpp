#pragma once

#include <ECS/Entity.hpp>
#include <Core.hpp>

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <qscrollarea.h>

#include "Widgets/Inspectors/InspectorWidgetBase.hpp"

class SectionContainer;
class RTTIInspectorWidget;
class TransformControl;
class StringControl;

using namespace Poly;

// This widget allows user change entity and its components properties.
// User also can add and remove any component.
// Updates object ASAP (to update inspector call UpdateInspector).
class EntityInspectorWidget : public InspectorWidgetBase
{
	Q_OBJECT

public:
	EntityInspectorWidget(QWidget* parent);

	// Initializes object connections with other inspectors and inspector manager.
	void InitializeConnections() override;

	// Sets default empty view.
	void Reset() override;

	// Updates every field within widget from currently set entity.
	// If there was any component added it will not show up after update.
	// If thre aren't any new component use this one rather than ReloadInspector.
	void UpdateInspector() override;

	// Releases all controls and creates everything again.
	// If any component was added you should use this one rather than UpdateInspector.
	void ReloadInspector() override;

	// Set entity to display; updates automatically.
	void SetSelectedEntities(Dynarray<Entity*> entities);

private:
	Dynarray<Entity*> Entities;

	QGridLayout* MainLayout;
	QMenu* ContextMenu;
		QAction* AddComponentAction;
		QAction* RemoveComponentAction;

	QLabel* NameText;			StringControl* NameField;
	QLabel* UniqueIdText;		QLineEdit* UniqueIdField;
	QLabel* ParentIdNameText;	QLineEdit* ParentIdNameField;	QPushButton* ParentChangeButton;	QPushButton* ParentSelectButton;
	QLabel* ChildrenIdNameText;	QComboBox* ChildrenIdNameField;	QPushButton* ChildrenSelectButton;

	SectionContainer* TransformSection;
	TransformControl* Transform;

	// Component sections
	Dynarray<SectionContainer*> ComponentSections;
	Dynarray<RTTIInspectorWidget*> ComponentInspectors;

private slots:
	void SpawnContextMenu(QPoint pos); 
		void AddComponent(); 
		void RemoveComponent();

	void ChangeParent();
	void SelectParent();
	void SelectChild();
};