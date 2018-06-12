#include "PolyEditorPCH.hpp"

//------------------------------------------------------------------------------
ResourceInspectorWidget::ResourceInspectorWidget(QWidget* parent)
	: InspectorWidgetBase(parent)
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
	connect(gApp->InspectorMgr, &InspectorManager::ProjectOpened, this, &ResourceInspectorWidget::SetObject);
	connect(gApp->InspectorMgr, &InspectorManager::ProjectClosed, this, &ResourceInspectorWidget::Reset);
}

//------------------------------------------------------------------------------
void ResourceInspectorWidget::Reset()
{
	Config = nullptr;
	delete Model;
	Model = new QFileSystemModel(this);
	Tree->setModel(Model);
}

//------------------------------------------------------------------------------
void ResourceInspectorWidget::SetObject(const ProjectConfig* config)
{
	Config = config;

	StringBuilder b;

	b.Append(Config->GetProjectPath());
	b.Append("/");
	b.Append(Config->GetProjectName());
	b.Append("/Res");

	QDir dir(b.StealString().GetCStr());
	Model->setRootPath(dir.absolutePath());
	Tree->setRootIndex(Model->index(dir.absolutePath()));
}
