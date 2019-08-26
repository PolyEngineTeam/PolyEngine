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
	connect(Manager, &InspectorManager::ProjectOpened, this, &ResourceInspectorWidget::ProjectOpened);
	connect(Manager, &InspectorManager::ProjectClosed, this, &ResourceInspectorWidget::ProjectClosed);
}

//------------------------------------------------------------------------------
void ResourceInspectorWidget::Reset()
{
	delete Model;
	Model = new QFileSystemModel(this);
	Tree->setModel(Model);
}



//		slots
//------------------------------------------------------------------------------
void ResourceInspectorWidget::ProjectOpened()
{
	core::storage::StringBuilder b;

	b.Append(Manager->GetConfig()->GetProjectPath());
	b.Append("/");
	b.Append(Manager->GetConfig()->GetProjectName());
	b.Append("/Res");

	QDir dir(b.StealString().GetCStr());
	Model->setRootPath(dir.absolutePath());
	Tree->setRootIndex(Model->index(dir.absolutePath()));
}

//------------------------------------------------------------------------------
void ResourceInspectorWidget::ProjectClosed()
{
	delete Model;
	Model = new QFileSystemModel(this);
	Tree->setModel(Model);
}
