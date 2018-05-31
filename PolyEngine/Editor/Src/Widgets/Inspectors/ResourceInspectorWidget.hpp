#pragma once

#include <QFormLayout>
#include <QTreeView>
#include <QFileSystemModel>

#include "Configs/ProjectConfig.hpp"
#include "Widgets/PolyWidget.hpp"

// File explorer with additional functionality, updates content ASAP.
class ResourceInspectorWidget : public PolyWidget
{
public:
	ResourceInspectorWidget();

	void SetObject(const ProjectConfig* config);

private:
	const ProjectConfig* Config;

	QGridLayout* Layout;

	QFileSystemModel* Model;
	QTreeView* Tree;

private slots:
	void Reset();
};