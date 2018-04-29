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

	// TODO(squares): use unique_ptr
	QListWidget* WidgetsList;

	// general data
	QGroupBox* GeneralDataGroup;
	QLabel* UniqueIdText;	QLineEdit* UniqueIdField;
	QLabel* ParentIdText;	QLineEdit* ParentIdField;	QPushButton* ParentIdButton;
	QLabel* ChildrenIdText;	QComboBox* ChildrenIdField;	QPushButton* ChildrenIdButton;

	// transform
	QGroupBox* TransformGroup;
							QLabel* XLabel;	QLabel* YLabel;	QLabel* ZLabel;
	QLabel* TranslationLabel;	QLineEdit* TranslationField[3][3];
	QLabel* RotationLabel;
	QLabel* ScaleLabel;

	// components
	QGroupBox* ComponentGroup;
	QLabel* ComponentText;
	QComboBox* ComponentField;
	Dynarray<QObject*> ComponentVars;
};