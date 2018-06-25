#include "PolyEditorPCH.hpp"

//		general
//------------------------------------------------------------------------------
ResourceInspectorWidget::ResourceInspectorWidget(QWidget* parent, InspectorManager* mgr)
	: InspectorWidgetBase(parent, mgr)
{
	Layout = new QGridLayout(this);

	Model = new QFileSystemModel(this);

	Tree = new QTreeView(this);
	Tree->setModel(Model);
	Layout->addWidget(Tree);
}

//------------------------------------------------------------------------------
void ResourceInspectorWidget::InitializeConnections()
{
	connect(gApp->InspectorMgr, &InspectorManager::ProjectOpened, this, &ResourceInspectorWidget::ProjectOpened);
	connect(gApp->InspectorMgr, &InspectorManager::ProjectClosed, this, &ResourceInspectorWidget::ProjectClosed);
}

void ResourceInspectorWidget::Reload()
{
	ProjectClosed();
	ProjectOpened();
}

//------------------------------------------------------------------------------
void ResourceInspectorWidget::Reset()
{
	Config = nullptr;
	delete Model;
	Model = new QFileSystemModel(this);
	Tree->setModel(Model);
}



//		slots
//------------------------------------------------------------------------------
void ResourceInspectorWidget::ProjectOpened()
{
	StringBuilder b;

	b.Append(Config->GetProjectPath());
	b.Append("/");
	b.Append(Config->GetProjectName());
	b.Append("/Res");

	QDir dir(b.StealString().GetCStr());
	Model->setRootPath(dir.absolutePath());
	Tree->setRootIndex(Model->index(dir.absolutePath()));
}

//------------------------------------------------------------------------------
void ResourceInspectorWidget::ProjectClosed()
{
	Config = nullptr;
	delete Model;
	Model = new QFileSystemModel(this);
	Tree->setModel(Model);
}
