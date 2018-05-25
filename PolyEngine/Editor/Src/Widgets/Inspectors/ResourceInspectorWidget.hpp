#pragma once

#include <QFormLayout>
#include <QTreeView>
#include <QFileSystemModel>

#include "Widgets/PolyWidget.hpp"

class ResourceInspectorWidget : public PolyWidget
{
public:
	ResourceInspectorWidget();

	void UpdateWidget();

private:
	QGridLayout* Layout;

	QFileSystemModel* Model;
	QTreeView* Tree;

private slots:
	void ProjectOpened();
};