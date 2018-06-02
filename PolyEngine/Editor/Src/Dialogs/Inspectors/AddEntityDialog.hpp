#pragma once

#include <qdialog.h>
#include <qlabel.h>
#include <qgridlayout.h>
#include <qlistwidget.h>
#include <qlineedit.h>

#include <ECS/World.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;

class AddEntityDialog : public QDialog
{
public:
	AddEntityDialog(World* world, Entity* parent = nullptr);

	bool OperationCanceled() { return Canceled; }

private:
	Entity* Parent;
	World* World;

	bool Canceled = false;

	QGridLayout* MainLayout;

	QListWidget* List;

	QPushButton* CancelButton;		QPushButton* OkButton;

private slots:
	void Ok();
	void Cancel();
};