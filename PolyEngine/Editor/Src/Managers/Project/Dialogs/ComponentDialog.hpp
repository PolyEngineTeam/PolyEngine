#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qtreewidget.h>

#include <pe/api/ecs/Scene.hpp>
#include <pe/api/ecs/Entity.hpp>

using namespace Poly;

class ComponentDialog : public QDialog
{
public:
	void AddComponents(Entity* entity);
	void RemoveComponents(Entity* entity);

	void AddWorldComponent(Scene* scene);
	void RemoveWorldComponent(Scene* scene);

	bool OperationCanceled() { return Canceled; }

private:
	enum class eMode
	{
		ADD,
		REMOVE,
		ADD_WORLD,
		REMOVE_WORLD,
		_COUNT
	};

	void InitUi(eMode mode);
	eMode Mode;
	Entity* EntityObj;
	Scene* SceneObj;
	bool Canceled = true;

	typedef void (*ComponentCreator)(::Entity* e);
	typedef void (*ComponentDestroyer)(::Entity* e);
	typedef void (*WorldComponentCreator)(::Scene* w);
	typedef void (*WorldComponentDestroyer)(::Scene* w);

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