#include "PolyEditorPCH.hpp"

#include <Configs/AssetsPathConfig.hpp>
#include "Utils/LibraryLoader.hpp"

//------------------------------------------------------------------------------
extern "C"
{
	using CreateGameFunc = IGame* (void);
}

static LibraryFunctionHandle<CreateGameFunc> LoadGame;

//------------------------------------------------------------------------------
void ProjectManager::Create(const String& projectName, const String& projectPath, const String& enginePath)
{
	if (ProjectCfg)
		throw new ProjectManagerException("Can't create new project without closing previous one.");

	StringBuilder builder;

	builder.Append("py ");
	builder.Append(enginePath);
	builder.Append("/Scripts/ProjectTool.py -c ");
	builder.Append(projectPath);
	builder.Append(" ");
	builder.Append(projectName);

	gApp->CommandMgr->RunCommand(builder.StealString());
}

//------------------------------------------------------------------------------
void ProjectManager::Open(const String& projectPath)
{
	if (ProjectCfg)
		throw new ProjectManagerException("Can't open project without closing previous one.");

	// create and load project file
	ProjectCfg = std::make_unique<ProjectConfig>(projectPath);
	ProjectCfg->Load();

	emit ProjectOpened(ProjectCfg.get());
}

//------------------------------------------------------------------------------
void ProjectManager::Update(const String& enginePath)
{
	if (!ProjectCfg)
		throw new ProjectManagerException("This operation requires any project opened.");

	StringBuilder builder;

	builder.Append("py ");
	builder.Append(enginePath);
	builder.Append("/Scripts/ProjectTool.py -u ");
	builder.Append(ProjectCfg->ProjectPath);

	gApp->CommandMgr->RunCommand(builder.GetString());
}

//------------------------------------------------------------------------------
void ProjectManager::Save()
{
}

//------------------------------------------------------------------------------
void ProjectManager::SaveAs(const String& path, const String& Name)
{

}

//------------------------------------------------------------------------------
void ProjectManager::Build()
{
	if (!ProjectCfg)
		throw new ProjectManagerException("This operation requires any project opened.");

	StringBuilder builder;

	builder.Append("cmake --build ");
	builder.Append(ProjectCfg->ProjectPath);
	builder.Append("/Build");

	gApp->CommandMgr->RunCommand(builder.GetString());
}

//------------------------------------------------------------------------------
void ProjectManager::Edit()
{
	if (!ProjectCfg)
		throw new ProjectManagerException("This operation requires any project opened.");

	if (gApp->EngineMgr->GetEngineState() == eEngineState::NONE)
		InitEngine();

	gApp->EngineMgr->Edit();
}

//------------------------------------------------------------------------------
void ProjectManager::Play()
{
	if (!ProjectCfg)
		throw new ProjectManagerException("This operation requires any project opened.");

	if (gApp->EngineMgr->GetEngineState() == eEngineState::NONE)
		InitEngine();
	
	// TODO(squares): fix problem with physics; Rigidbody and collider components  are initialized in next frame 
	//		so when next frame never occur we try to delete empty ImplData
	gApp->EngineMgr->Play();
}

//------------------------------------------------------------------------------
void ProjectManager::Close()
{
	if (!ProjectCfg)
		throw new ProjectManagerException("This operation requires any project opened.");

	gApp->EngineMgr->DeinitEngine();
	ProjectCfg.release();
	emit ProjectClosed();
}

//------------------------------------------------------------------------------
void ProjectManager::InitEngine()
{
	// load game
	if (!LoadGame.FunctionValid())
	{
		LoadGame = LoadFunctionFromSharedLibrary<CreateGameFunc>(ProjectCfg->GetGameDllPath().GetCStr(), "CreateGame");
		ASSERTE(LoadGame.FunctionValid(), "Library libGame load failed");
		gConsole.LogDebug("Library libGame loaded.");
	}

	std::unique_ptr<IGame> game = std::unique_ptr<IGame>(LoadGame());

	// works for VS
	StringBuilder builder;
	builder.Append(ProjectCfg->ProjectPath);
	builder.Append("/Build/");
	builder.Append(ProjectCfg->ProjectName);
	builder.Append("/Debug/AssetsPathConfig.json");

	gApp->EngineMgr->InitEngine(std::move(game), builder.GetString());
}
