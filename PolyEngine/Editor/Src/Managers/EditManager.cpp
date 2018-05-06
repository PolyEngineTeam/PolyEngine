#include <PolyEditorPCH.hpp>

EditManager::EditManager()
{
	connect(gApp->Ui.WorldExplorer, &WorldExplorerWidget::EntitySelected, this, &EditManager::SelectedEntityChanged);
}

void EditManager::SelectedEntityChanged(Entity* entity)
{
	gApp->Ui.EntityMgr->SetEntity(entity);
}
