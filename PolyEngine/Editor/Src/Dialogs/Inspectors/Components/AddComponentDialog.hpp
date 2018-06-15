#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qtreewidget.h>

#include <QtWidgets/qtreewidget.h>

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;

class AddComponentDialog : public QDialog
{
public:
	AddComponentDialog(Entity* entity);
	AddComponentDialog(World* world);

	bool OperationCanceled() { return Canceled; }

private:
	void InitControls();

	Entity* EntityObj = nullptr;
	World* WorldObj = nullptr;
	bool Canceled = false;

	typedef void (*ComponentCreator)(::Entity* e);
	typedef void (*WorldComponentCreator)(::World* w);
	std::map<QString, ComponentCreator> ComponentCreators;
	std::map<QString, WorldComponentCreator> WorldComponentCreators;

	QGridLayout* MainLayout;

	QTreeWidget* Tree;

	QLabel* EntityIdNameText;		QLineEdit* EntityIdNameField;
	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};