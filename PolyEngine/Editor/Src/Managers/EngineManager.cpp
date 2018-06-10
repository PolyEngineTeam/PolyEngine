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
	if (Engine)
		throw new std::exception("Creating Engine twice?");

	// get editor inserface and set path to assets
	Editor = gApp->InspectorMgr->GetEditor();
	Editor->SetAssetsPathConfigPath(assetsPathConfigPath);
	Editor->SetEngineState(eEngineState::EDIT);

	// create and initialize engine instance
	Engine = std::make_unique<Poly::Engine>();
	Engine->RegisterEditor(Editor);
	Engine->Init(std::move(game), std::move(gApp->InspectorMgr->GetRenderingDevice()));
	gConsole.LogDebug("Engine initialized successfully");
	Updater.start(0);

	emit Initialized(Engine->GetWorld());
}

//------------------------------------------------------------------------------
void EngineManager::DeinitEngine()
{
	Engine.release();

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
		throw new std::exception("To switch to edit mode engine must be initialized.");

	case eEngineState::EDIT:
		// do nothing
		break;

	case eEngineState::GAMEPLAY:
		Editor->SetEngineState(eEngineState::EDIT);
		Engine->Restart();
		break;

	default:
		throw new std::exception("Unhandled engine state.");
	}

	emit StateChanged(eEngineState::EDIT);
}

//------------------------------------------------------------------------------
void EngineManager::Play()
{
	switch (Editor->GetEngineState())
	{
	case eEngineState::NONE:
		throw new std::exception("To switch to gameplay mode engine must be initialized");

	case eEngineState::EDIT:
		Editor->SetEngineState(eEngineState::GAMEPLAY);
		Engine->Restart();
		break;

	case eEngineState::GAMEPLAY:
		// do nothing
		break;

	default:
		throw new std::exception("Unhandled engine state.");
	}

	emit StateChanged(eEngineState::GAMEPLAY);
}

//------------------------------------------------------------------------------
void EngineManager::UpdatePhase()
{
	Dynarray<Engine::eUpdatePhaseOrder> updatePhases;

	//updatePhases.PushBack(Engine::eUpdatePhaseOrder::PREUPDATE);
	//updatePhases.PushBack(Engine::eUpdatePhaseOrder::EDITOR);
	//updatePhases.PushBack(Engine::eUpdatePhaseOrder::POSTUPDATE);

	if (Editor->GetEngineState() == eEngineState::EDIT)
		Engine->Update(updatePhases);
	else
		Engine->Update();
}