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
// User also can add and remove any widget.
class EntityManagerWidget : public PolyWidget
{
public:
	EntityManagerWidget();
	~EntityManagerWidget();

	// Set entity to display; updates automatically.
	// @param entity - entity to display
	void SetEntity(Entity* entity);

	// Updates every property of entity and its components from widget fields.
	void UpdateEntity();
	// Updates every field within widget from currently set entity.
	void UpdateWidget();

private:
	Entity* Entity;

	std::unique_ptr<QScrollArea> ScrollArea;
	std::unique_ptr<QGridLayout> MainLayout;

	std::unique_ptr<QLabel> NameText;			std::unique_ptr<QLineEdit> NameField;
	std::unique_ptr<QLabel> UniqueIdText;		std::unique_ptr<QLineEdit> UniqueIdField;
	std::unique_ptr<QLabel> ParentIdNameText;	std::unique_ptr<QLineEdit> ParentIdNameField;	std::unique_ptr<QPushButton> ParentIdNameButton;
	std::unique_ptr<QLabel> ChildrenIdNameText;	std::unique_ptr<QComboBox> ChildrenIdNameField;	std::unique_ptr<QPushButton> ChildrenIdNameButton;

	std::unique_ptr<SectionContainer> TransformSection;
	std::unique_ptr<QLabel> XLabel;	std::unique_ptr<QLabel> YLabel;	std::unique_ptr<QLabel> ZLabel;
	std::unique_ptr<QLabel> TranslationLabel;	std::unique_ptr<QLineEdit> TranslationField[3][3];	//	[0][0]  [0][1]  [0][2]
	std::unique_ptr<QLabel> RotationLabel;															//  [1][0]  [1][1]  [1][2]
	std::unique_ptr<QLabel> ScaleLabel;																//  [2][0]  [2][1]  [2][2]

	// Component sections
	Dynarray<SectionContainer*> ComponentSections;
	Dynarray<RTTIViewerWidget*> ComponentWidgets;
};