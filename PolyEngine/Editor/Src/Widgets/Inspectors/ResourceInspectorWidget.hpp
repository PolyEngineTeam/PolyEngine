#pragma once

// Temporary namespace usings
using namespace pe;

#include <QFormLayout>
#include <QTreeView>
#include <QFileSystemModel>

#include "Configs/ProjectConfig.hpp"
#include "Widgets/Inspectors/InspectorWidgetBase.hpp"

// File explorer with additional functionality, updates content ASAP.
class ResourceInspectorWidget : public InspectorWidgetBase
{
public:
	ResourceInspectorWidget(QWidget* parent, InspectorManager* mgr);

	// Initializes object connections with other inspectors and inspector manager.
	void InitializeConnections() override;

	// Removes all items from viewer.
	void Reset() override;

public slots:
	void ProjectOpened();
	void ProjectClosed();

private:
	QGridLayout* Layout;

	QFileSystemModel* Model;
	QTreeView* Tree;
};