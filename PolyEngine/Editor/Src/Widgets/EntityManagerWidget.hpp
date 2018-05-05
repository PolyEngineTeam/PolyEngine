#pragma once

#include <ECS/Entity.hpp>
#include <Core.hpp>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>
#include <qgroupbox.h>

#include "Widgets/PolyWidget.hpp"
#include "Containers/SectionContainer.hpp"

using namespace Poly;

class EntityManagerWidget : public PolyWidget
{
public:
	EntityManagerWidget();
	~EntityManagerWidget();

	void SetEntity(Entity* entity);
	void UpdateWidget();
	void UpdateEntity();

private:
	Entity* Entity;

	std::unique_ptr<QLayout> Layout;

	// general data
	QLabel* NameText;			QLineEdit* NameField;
	QLabel* UniqueIdText;		QLineEdit* UniqueIdField;
	QLabel* ParentIdNameText;	QLineEdit* ParentIdNameField;	QPushButton* ParentIdNameButton;
	QLabel* ChildrenIdNameText;	QComboBox* ChildrenIdNameField;	QPushButton* ChildrenIdNameButton;

	// transform
	SectionContainer* TransformSection;
							QLabel* XLabel;	QLabel* YLabel;	QLabel* ZLabel;
	QLabel* TranslationLabel;	QLineEdit* TranslationField[3][3];		//	[0][0]  [0][1]  [0][2]
	QLabel* RotationLabel;												//  [1][0]  [1][1]  [1][2]
	QLabel* ScaleLabel;													//  [2][0]  [2][1]  [2][2]

	// components
	Dynarray<SectionContainer*> Components;
};