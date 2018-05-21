#include "PolyEditorPCH.hpp"

ResourceManagerWidget::ResourceManagerWidget()
{
	Layout = new QGridLayout(this);

	Model = new QFileSystemModel(this);

	Tree = new QTreeView(this);
	Tree->setModel(Model);
	Layout->addWidget(Tree);

	connect(&gApp->ProjectMgr, &ProjectManager::ProjectOpened, this, &ResourceManagerWidget::ProjectOpened);
}

void ResourceManagerWidget::UpdateWidget()
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

void ResourceManagerWidget::ProjectOpened()
{
	UpdateWidget();
}
