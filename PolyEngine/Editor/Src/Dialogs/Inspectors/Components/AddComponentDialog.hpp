#pragma once

#include <qdialog.h>
#include <qlabel.h>
#include <QtWidgets/qgridlayout.h>
#include <qlistwidget.h>
#include <QtWidgets/qlineedit.h>
#include <qtreewidget.h>
#include <qpushbutton.h>

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

	Entity* Entity = nullptr;
	World* World = nullptr;
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