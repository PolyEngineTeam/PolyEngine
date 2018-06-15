#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qtreewidget.h>
#include <QtWidgets/qpushbutton.h>

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;

class RemoveComponentDialog : public QDialog
{
public:
	RemoveComponentDialog(Entity* entity);
	RemoveComponentDialog(World* world);

	bool OperationCanceled() { return Canceled; }

private:
	void InitControls();

	Entity* EntityObj = nullptr;
	World* WorldObj = nullptr;
	bool Canceled = false;

	typedef void (*ComponentDestroyer)(::Entity* e);
	typedef void (*WorldComponentDestroyer)(::World* w);
	std::map<QString, ComponentDestroyer> ComponentDestroyers;
	std::map<QString, WorldComponentDestroyer> WorldComponentDestroyers;

	QGridLayout* MainLayout;

	QTreeWidget* Tree;

	QLabel* EntityIdNameText;		QLineEdit* EntityIdNameField;
	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};