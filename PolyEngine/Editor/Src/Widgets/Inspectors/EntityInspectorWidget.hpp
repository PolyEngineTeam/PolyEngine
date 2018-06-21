#pragma once

#include <ECS/Entity.hpp>
#include <Core.hpp>

#include <QtWidgets/qlabel.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>

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

public slots:
	// Sets selected entities.
	void EntitiesSelectionChanged(Dynarray<Entity*> entities);

	// Updates fields within sections and general data fields.
	void SoftUpdate();

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