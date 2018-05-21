#pragma once

#include <QFormLayout>
#include <QTreeView>
#include <QFileSystemModel>

#include "Widgets/PolyWidget.hpp"

class ResourceManagerWidget : public PolyWidget
{
public:
	ResourceManagerWidget();

	void UpdateWidget();

private:
	QGridLayout* Layout;

	QFileSystemModel* Model;
	QTreeView* Tree;

private slots:
	void ProjectOpened();
};