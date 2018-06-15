#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlineedit.h>

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;

class AddEntityDialog : public QDialog
{
public:
	AddEntityDialog(World* world, Entity* parent = nullptr);

	Entity* GetResult() { return Result; }
	bool OperationCanceled() { return Canceled; }

private:
	Entity* Parent;
	World* WorldObj;

	Entity* Result = nullptr;
	bool Canceled = false;

	QGridLayout* MainLayout;
	QListWidget* List;
	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};