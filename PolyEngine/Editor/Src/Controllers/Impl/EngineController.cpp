#include <PolyEditorPCH.hpp>

#include <Editor/IEditor.hpp>
#include <pe/core/utils/LibraryLoader.hpp>

using namespace editor::controllers::impl;

//------------------------------------------------------------------------------
extern "C"
{
	using CreateGameFunc = IGame * (void);
}

static core::utils::LibraryFunctionHandle<CreateGameFunc> LoadGame;

//------------------------------------------------------------------------------
std::unique_ptr<EngineController> EngineController::Create(const ProjectConfig& projectConfig)
{

	// load game
	if (!LoadGame.FunctionValid())
	{
		const core::storage::String libGamePath = projectConfig.GetGameDllPath();
		LoadGame = core::utils::LoadFunctionFromSharedLibrary<CreateGameFunc>(libGamePath.GetCStr(), "CreateGame");
		ASSERTE(LoadGame.FunctionValid(), "Library libGame load failed");
		core::utils::gConsole.LogDebug("Library libGame loaded from {}.", libGamePath);
	}

	std::unique_ptr<IGame> game = std::unique_ptr<IGame>(LoadGame());

	auto engine = std::make_unique<Poly::Engine>(false, gApp->InspectorMgr->GetEditor());

	engine->Init(std::move(game), gApp->InspectorMgr->GetRenderingDevice());
	core::utils::gConsole.LogDebug("Engine initialized successfully");

	auto result = std::make_unique<EngineController>();
	result->SetEngine(std::move(engine));

	return result;
}

//------------------------------------------------------------------------------
EngineController::EngineController()
{
	// connect timer to function that will update engine
	EngineUpdateTimer.connect(&EngineUpdateTimer, &QTimer::timeout, [this]() { OnEngineUpdateTimerTick(); });
}

//------------------------------------------------------------------------------
void EngineController::SetEngine(std::unique_ptr<Poly::Engine> engineToControl)
{
	ControlledEngine = std::move(engineToControl);
	Editor = static_cast<PolyEditor::IEditor*>(ControlledEngine->GetEditor());
}

//------------------------------------------------------------------------------
void EngineController::Edit()
{
	ASSERTE(ControlledEngine, "Engine instance is required to perform this operation");

	EngineUpdateTimer.stop();
	const auto oldEngineState = Editor->GetEngineState();
	Editor->SetEngineState(PolyEditor::eEngineState::EDIT);

	switch (oldEngineState)
	{
	case PolyEditor::eEngineState::NONE:
		ControlledEngine->StartGame();
		break;

	case PolyEditor::eEngineState::EDIT:
	case PolyEditor::eEngineState::GAMEPLAY:
	case PolyEditor::eEngineState::PAUSED_GAMEPLAY:
		ControlledEngine->EndGame();
		ControlledEngine->StartGame();
		break;

	default:
		ASSERTE(false, "Other values not supported.");
	}

	EngineUpdateTimer.start(0);
}

//------------------------------------------------------------------------------
void EngineController::Play()
{
	ASSERTE(ControlledEngine, "Engine instance is required to perform this operation");

	EngineUpdateTimer.stop();
	const auto oldEngineState = Editor->GetEngineState();
	Editor->SetEngineState(PolyEditor::eEngineState::GAMEPLAY);

	switch (oldEngineState)
	{
	case PolyEditor::eEngineState::NONE:
		ControlledEngine->StartGame();
		break;

	case PolyEditor::eEngineState::GAMEPLAY:
	case PolyEditor::eEngineState::EDIT:
		ControlledEngine->EndGame();
		ControlledEngine->StartGame();
		break;

	case PolyEditor::eEngineState::PAUSED_GAMEPLAY:
		ControlledEngine->StartGame();
		break;

	default:
		ASSERTE(false, "Other values not supported.");
	}

	EngineUpdateTimer.start(0);
}

//------------------------------------------------------------------------------
void EngineController::Pause()
{
	ASSERTE(ControlledEngine, "Engine instance is required to perform this operation");

	EngineUpdateTimer.stop();
	const auto oldEngineState = Editor->GetEngineState();
	Editor->SetEngineState(PolyEditor::eEngineState::PAUSED_GAMEPLAY);

	switch (oldEngineState)
	{
	case PolyEditor::eEngineState::NONE:
		ControlledEngine->StartGame();
		break;

	case PolyEditor::eEngineState::EDIT:
		ControlledEngine->EndGame();
		ControlledEngine->StartGame();
		break;

	case PolyEditor::eEngineState::GAMEPLAY:
	case PolyEditor::eEngineState::PAUSED_GAMEPLAY:
		// do nothing
		break;

	default:
		ASSERTE(false, "Other values not supported.");
	}

	EngineUpdateTimer.start(0);
}

//------------------------------------------------------------------------------
void EngineController::OnEngineUpdateTimerTick()
{
	ASSERTE(ControlledEngine, "Engine instance is required to perform this operation");

	auto updatePhases = std::vector<Engine::eUpdatePhaseOrder>();

	switch (Editor->GetEngineState())
	{
	case PolyEditor::eEngineState::EDIT:
		updatePhases.push_back(Engine::eUpdatePhaseOrder::PREUPDATE);
		updatePhases.push_back(Engine::eUpdatePhaseOrder::EDITOR);
		updatePhases.push_back(Engine::eUpdatePhaseOrder::POSTUPDATE);
		break;

	case PolyEditor::eEngineState::GAMEPLAY:
		updatePhases.push_back(Engine::eUpdatePhaseOrder::PREUPDATE);
		updatePhases.push_back(Engine::eUpdatePhaseOrder::UPDATE);
		updatePhases.push_back(Engine::eUpdatePhaseOrder::POSTUPDATE);
		break;

	case PolyEditor::eEngineState::PAUSED_GAMEPLAY:
		updatePhases.push_back(Engine::eUpdatePhaseOrder::PREUPDATE);
		updatePhases.push_back(Engine::eUpdatePhaseOrder::EDITOR);
		updatePhases.push_back(Engine::eUpdatePhaseOrder::POSTUPDATE);
		break;

	default:
		ASSERTE(false, "Other values not supported.");
	}

	gApp->InspectorMgr->GetViewportInspector()->HandleEngineOutputEvents(ControlledEngine->GetOutputQueue());

	ControlledEngine->Update(updatePhases);
}
