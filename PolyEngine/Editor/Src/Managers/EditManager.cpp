#include "PolyEditorPCH.hpp"

EditManager::EditManager()
{
}

void EditManager::SelectedEntityChanged(Entity* entity)
{
	gApp->Ui.EntityMgr->SetObject(entity);
}
