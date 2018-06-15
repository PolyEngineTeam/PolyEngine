#include "PolyEditorPCH.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
EngineManager::EngineManager()
	: Updater(this)
{
	// setup update timer
	connect(&Updater, &QTimer::timeout, this, &EngineManager::UpdatePhase);
}

//------------------------------------------------------------------------------
void EngineManager::InitEngine(std::unique_ptr<IGame> game, const String& assetsPathConfigPath)
{
	if (EngineObj)
		throw new std::exception((const char*)"Creating Engine twice?");

	// get editor inserface and set path to assets
	Editor = gApp->InspectorMgr->GetEditor();
	Editor->SetAssetsPathConfigPath(assetsPathConfigPath);
	Editor->SetEngineState(eEngineState::EDIT);

	// create and initialize engine instance
	EngineObj = std::make_unique<Poly::Engine>();
	EngineObj->RegisterEditor(Editor);
	EngineObj->Init(std::move(game), std::move(gApp->InspectorMgr->GetRenderingDevice()));
	gConsole.LogDebug("Engine initialized successfully");
	Updater.start(0);

	emit Initialized(EngineObj->GetWorld());
}

//------------------------------------------------------------------------------
void EngineManager::DeinitEngine()
{
	EngineObj.release();

	Editor->SetEngineState(eEngineState::NONE);
	Editor = nullptr;
	Updater.stop();

	emit Deinitialized();
}

//------------------------------------------------------------------------------
void EngineManager::Edit()
{
	switch (Editor->GetEngineState())
	{
	case eEngineState::NONE:
		throw new std::exception((const char*)"To switch to edit mode engine must be initialized.");

	case eEngineState::EDIT:
		// do nothing
		break;

	case eEngineState::GAMEPLAY:
		Editor->SetEngineState(eEngineState::EDIT);
		EngineObj->Restart();
		break;

	default:
		throw new std::exception((const char*)"Unhandled engine state.");
	}

	emit StateChanged(eEngineState::EDIT);
}

//------------------------------------------------------------------------------
void EngineManager::Play()
{
	switch (Editor->GetEngineState())
	{
	case eEngineState::NONE:
		throw new std::exception((const char*)"To switch to gameplay mode engine must be initialized");

	case eEngineState::EDIT:
		Editor->SetEngineState(eEngineState::GAMEPLAY);
		EngineObj->Restart();
		break;

	case eEngineState::GAMEPLAY:
		// do nothing
		break;

	default:
		throw new std::exception((const char*)"Unhandled engine state.");
	}

	emit StateChanged(eEngineState::GAMEPLAY);
}

//------------------------------------------------------------------------------
void EngineManager::UpdatePhase()
{
	Dynarray<Engine::eUpdatePhaseOrder> updatePhases;

	if (Editor->GetEngineState() == eEngineState::EDIT)
	{
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::PREUPDATE);
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::EDITOR);
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::POSTUPDATE);
	}
	else
	{
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::PREUPDATE);
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::UPDATE);
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::POSTUPDATE);
	}

	EngineObj->Update(updatePhases);
}