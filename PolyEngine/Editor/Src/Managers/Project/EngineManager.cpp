#include "PolyEditorPCH.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
EngineManager::EngineManager()
	: Updater(this), EditorUpdater(this)
{
	// setup update timer
	connect(&Updater, &QTimer::timeout, this, &EngineManager::UpdatePhase);
	connect(&EditorUpdater, &QTimer::timeout, this, &EngineManager::EditorUpdatePhase);
}

//------------------------------------------------------------------------------
void EngineManager::InitEngine(std::unique_ptr<IGame> game, const String& assetsPathConfigPath)
{
	// get editor inserface and set path to assets
	Editor = gApp->InspectorMgr->GetEditor();

	// create and initialize engine instance
	EngineObj = std::make_unique<Poly::Engine>(false, Editor);
	emit Created(EngineObj.get());

	EngineObj->Init(std::move(game), gApp->InspectorMgr->GetRenderingDevice());
	gConsole.LogDebug("Engine initialized successfully");

	emit Initialized();

	Editor->SetEngineState(PolyEditor::eEngineState::EDIT);
}

//------------------------------------------------------------------------------
void EngineManager::DeinitEngine()
{
	EngineObj.reset();

	Editor->SetEngineState(PolyEditor::eEngineState::NONE);
	Editor = nullptr;
	Updater.stop();
	EditorUpdater.stop();

	emit Deinitialized();
}

//------------------------------------------------------------------------------
void EngineManager::Edit()
{
	Updater.stop();
	EditorUpdater.stop();
	switch (Editor->GetEngineState())
	{
	case PolyEditor::eEngineState::NONE:
		throw new std::exception();

	case PolyEditor::eEngineState::EDIT:
		// do nothing
		break;

	case PolyEditor::eEngineState::GAMEPLAY:
		Editor->SetEngineState(PolyEditor::eEngineState::EDIT);
		EngineObj->EndGane();
		EngineObj->StartGame();
		break;

	default:
		throw new std::exception();
	}

	emit Initialized();
	emit StateChanged(PolyEditor::eEngineState::EDIT);
	Updater.start(0);
}

//------------------------------------------------------------------------------
void EngineManager::Play()
{
	Updater.stop();
	EditorUpdater.stop();
	switch (Editor->GetEngineState())
	{
	case PolyEditor::eEngineState::NONE:
		throw new std::exception();

	case PolyEditor::eEngineState::EDIT:
		Editor->SetEngineState(PolyEditor::eEngineState::GAMEPLAY);
		EngineObj->EndGane();
		EngineObj->StartGame();
		break;

	case PolyEditor::eEngineState::GAMEPLAY:
		EngineObj->EndGane();
		EngineObj->StartGame();
		break;

	default:
		throw new std::exception();
	}

	emit Initialized();
	emit StateChanged(PolyEditor::eEngineState::GAMEPLAY);
	Updater.start(0);
	EditorUpdater.start(250);
}

//------------------------------------------------------------------------------
void EngineManager::UpdatePhase()
{
	if (!EngineObj)
		return;

	Dynarray<Engine::eUpdatePhaseOrder> updatePhases;

	if (Editor->GetEngineState() == PolyEditor::eEngineState::EDIT)
	{
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::PREUPDATE);
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::EDITOR);
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::POSTUPDATE);
	}
	else if (Editor->GetEngineState() == PolyEditor::eEngineState::GAMEPLAY)
	{
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::PREUPDATE);
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::UPDATE);
		updatePhases.PushBack(Engine::eUpdatePhaseOrder::POSTUPDATE);
	}

	EngineObj->Update(updatePhases);
}

//------------------------------------------------------------------------------
void EngineManager::EditorUpdatePhase()
{
	emit gApp->InspectorMgr->Update();
}