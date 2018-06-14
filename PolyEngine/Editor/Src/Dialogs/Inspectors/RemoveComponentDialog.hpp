#pragma once

#include <qdialog.h>
#include <qlabel.h>
#include <qgridlayout.h>
#include <qlistwidget.h>
#include <qlineedit.h>

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;

class RemoveComponentDialog : public QDialog
{
public:
	RemoveComponentDialog(Entity* entity);

	bool OperationCanceled() { return Canceled; }

private:
	Entity * entity;;

	bool Canceled = false;

	QGridLayout* MainLayout;
	QListWidget* List;
	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};