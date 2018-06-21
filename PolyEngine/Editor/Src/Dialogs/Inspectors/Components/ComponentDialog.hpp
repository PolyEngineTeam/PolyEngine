#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qtreewidget.h>

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;

class ComponentDialog : public QDialog
{
public:
	void AddComponent(Entity* entity);
	void RemoveComponent(Entity* entity);

	void AddWorldComponent(World* world);
	void RemoveWorldComponent(World* world);

	bool OperationCanceled() { return Canceled; }

private:
	enum class eMode
	{
		ADD,
		REMOVE,
		_COUNT
	};

	void InitUi(eMode mode);
	bool Canceled = true;

	typedef void (*ComponentCreator)(::Entity* e);
	typedef void(*ComponentDestroyer)(::Entity* e);
	typedef void (*WorldComponentCreator)(::World* w);
	typedef void(*WorldComponentDestroyer)(::World* w);

	std::map<QString, ComponentCreator> ComponentCreators;
	std::map<QString, WorldComponentCreator> WorldComponentCreators;
	std::map<QString, ComponentDestroyer> ComponentDestroyers;
	std::map<QString, WorldComponentDestroyer> WorldComponentDestroyers;

	QGridLayout* MainLayout;

	QTreeWidget* ComponentsTree;

	QLabel* EntityIdNameText;		QLineEdit* EntityIdNameField;
	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};