#include "PolyEditorPCH.hpp"

//------------------------------------------------------------------------------
ProjectManager::ProjectManager(std::unique_ptr<ProjectConfig> projectCfg)
	: ProjectCfg(std::move(projectCfg))
{
}

//------------------------------------------------------------------------------
void ProjectManager::Edit()
{
	if (!gApp->EngineController)
		InitEngine();

	gApp->EngineController->Edit();

	emit EngineStateChanged(PolyEditor::eEngineState::EDIT);
}

//------------------------------------------------------------------------------
void ProjectManager::Play()
{
	if (!gApp->EngineController)
		InitEngine();
	
	// TODO(squares): fix problem with physics; Rigidbody and collider components  are initialized in next frame 
	//		so when next frame never occur we try to delete empty ImplData
	gApp->EngineController->Play();

	emit EngineStateChanged(PolyEditor::eEngineState::GAMEPLAY);
}

//------------------------------------------------------------------------------
void ProjectManager::InitEngine()
{
	auto controller = editor::controllers::impl::EngineController::Create(*ProjectCfg);
	emit EngineInitialized(controller->GetEngine());
	gApp->EngineController = std::move(controller);
}
