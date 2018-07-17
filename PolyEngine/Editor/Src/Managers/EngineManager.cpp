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
	// get editor inserface and set path to assets
	Editor = gApp->InspectorMgr->GetEditor();

	// create and initialize engine instance
	EngineObj = std::make_unique<Poly::Engine>();
	emit Created(EngineObj.get());

	EngineObj->RegisterEditor(Editor);
	EngineObj->Init(std::move(game), std::move(gApp->InspectorMgr->GetRenderingDevice()));
	gConsole.LogDebug("Engine initialized successfully");

	emit Initialized();

	Editor->SetEngineState(eEngineState::EDIT);
}

//------------------------------------------------------------------------------
void EngineManager::DeinitEngine()
{
	EngineObj.reset();

	Editor->SetEngineState(eEngineState::NONE);
	Editor = nullptr;
	Updater.stop();

	emit Deinitialized();
}

//------------------------------------------------------------------------------
void EngineManager::Edit()
{
	Updater.stop();
	switch (Editor->GetEngineState())
	{
	case eEngineState::NONE:
		throw new std::exception();

	case eEngineState::EDIT:
		// do nothing
		break;

	case eEngineState::GAMEPLAY:
		Editor->SetEngineState(eEngineState::EDIT);
		EngineObj->Restart();
		break;

	default:
		throw new std::exception();
	}

	emit StateChanged(eEngineState::EDIT);
	Updater.start(0);
}

//------------------------------------------------------------------------------
void EngineManager::Play()
{
	Updater.stop();
	switch (Editor->GetEngineState())
	{
	case eEngineState::NONE:
		throw new std::exception();

	case eEngineState::EDIT:
		Editor->SetEngineState(eEngineState::GAMEPLAY);
		EngineObj->Restart();
		break;

	case eEngineState::GAMEPLAY:
		EngineObj->Restart();
		break;

	default:
		throw new std::exception();
	}

	emit StateChanged(eEngineState::GAMEPLAY);
	Updater.start(0);
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
	else if (Editor->GetEngineState() == eEngineState::GAMEPLAY)
	{
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::PREUPDATE);
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::UPDATE);
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::POSTUPDATE);
	}

	EngineObj->Update(updatePhases);
}