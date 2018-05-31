#include "PolyEditorPCH.hpp"

ResourceInspectorWidget::ResourceInspectorWidget()
{
	connect(&gApp->InspectorMgr, &InspectorManager::EngineInitialized, this, &ResourceInspectorWidget::EngineInitialized);
	
	Layout = new QGridLayout(this);

	Model = new QFileSystemModel(this);

	Tree = new QTreeView(this);
	Tree->setModel(Model);
	Layout->addWidget(Tree);
}

void ResourceInspectorWidget::UpdateWidget()
{
	StringBuilder b;

	b.Append(gApp->ProjectMgr.GetProjectConfig().GetProjectPath());
	b.Append("/");
	b.Append(gApp->ProjectMgr.GetProjectConfig().GetProjectName());
	b.Append("/Res");

	QDir dir(b.StealString().GetCStr());
	Model->setRootPath(dir.absolutePath());
	Tree->setRootIndex(Model->index(dir.absolutePath()));
}

void ResourceInspectorWidget::EngineInitialized()
{
	UpdateWidget();
}
