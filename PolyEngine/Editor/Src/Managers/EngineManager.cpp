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

	EngineState = eEngineState::EDIT;

	// get editor inserface and set path to assets
	IEditor* ed = gApp->InspectorMgr->GetEditor();
	ed->SetAssetsPathConfigPath(assetsPathConfigPath);

	// create and initialize engine instance
	Engine = std::make_unique<Poly::Engine>();
	Engine->RegisterEditor(ed);
	Engine->Init(std::move(game), std::move(gApp->InspectorMgr->GetRenderingDevice()));
	gConsole.LogDebug("Engine initialized successfully");
	Updater.start(0);

	emit Initialized(Engine->GetWorld());
}

//------------------------------------------------------------------------------
void EngineManager::DeinitEngine()
{
	Engine.release();

	EngineState = eEngineState::NONE;
	Updater.stop();

	emit Deinitialized();
}

//------------------------------------------------------------------------------
void EngineManager::Edit()
{
	switch (EngineState)
	{
	case eEngineState::NONE:
		throw new std::exception("To switch to edit mode engine must be initialized.");

	case eEngineState::EDIT:
		// do nothing
		break;

	case eEngineState::GAMEPLAY:
		EngineState = eEngineState::EDIT;
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
	switch (EngineState)
	{
	case eEngineState::NONE:
		throw new std::exception("To switch to gameplay mode engine must be initialized");

	case eEngineState::EDIT:
		EngineState = eEngineState::GAMEPLAY;
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

	if (EngineState == eEngineState::EDIT)
		Engine->Update(updatePhases);
	else
		Engine->Update();
}