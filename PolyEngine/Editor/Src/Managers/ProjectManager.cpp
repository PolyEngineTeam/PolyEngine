#include "PolyEditorPCH.hpp"

#include <Configs/AssetsPathConfig.hpp>
#include "Utils/LibraryLoader.hpp"

extern "C"
{
	using CreateGameFunc = IGame* (void);
}

static LibraryFunctionHandle<CreateGameFunc> LoadGame;

void ProjectManager::Create(const String& projectName, const String& projectPath, const String& enginePath)
{
	if (ProjectConfig)
		throw new ProjectManagerException("Can't create new project without closing previous one.");

	StringBuilder builder;

	builder.Append("py ");
	builder.Append(enginePath);
	builder.Append("/Scripts/ProjectTool.py -e ");
	builder.Append(enginePath);
	builder.Append(" -c ");
	builder.Append(projectPath);
	builder.Append(" ");
	builder.Append(projectName);

	gApp->CommandMgr.RunCommand(builder.StealString());
}

void ProjectManager::Open(String projectPath)
{
	if (ProjectConfig)
		throw new ProjectManagerException("Can't open project without closing previous one.");

	// create and load project file
	ProjectConfig = std::make_unique<::ProjectConfig>(projectPath);
	ProjectConfig->Load();
}

void ProjectManager::Update(const String& enginePath)
{
	if (!ProjectConfig)
		throw new ProjectManagerException("This operation requires any project opened.");

	StringBuilder builder;

	builder.Append("py ");
	builder.Append(enginePath);
	builder.Append("/Scripts/ProjectTool.py -e ");
	builder.Append(enginePath);
	builder.Append(" -u ");
	builder.Append(ProjectConfig->ProjectPath);

	gApp->CommandMgr.RunCommand(builder.GetString());
}

void ProjectManager::Build()
{
	if (!ProjectConfig)
		throw new ProjectManagerException("This operation requires any project opened.");

	StringBuilder builder;

	builder.Append("cmake --build ");
	builder.Append(ProjectConfig->ProjectPath);
	builder.Append("/Build");

	gApp->CommandMgr.RunCommand(builder.GetString());
}

void ProjectManager::Edit()
{
	if (!ProjectConfig)
		throw new ProjectManagerException("This operation requires any project opened.");

	if (gApp->EngineMgr.GetEngineState() == eEngineState::NONE)
	{
		// load game
		if (!LoadGame.FunctionValid())
		{
			LoadGame = LoadFunctionFromSharedLibrary<CreateGameFunc>(ProjectConfig->GetGameDllPath().GetCStr(), "CreateGame");
			ASSERTE(LoadGame.FunctionValid(), "Library libGame load failed");
			gConsole.LogDebug("Library libGame loaded.");
		}

		std::unique_ptr<IGame> game = std::unique_ptr<IGame>(LoadGame());
		std::unique_ptr<IRenderingDevice> device = gApp->Ui.MainViewport->GetRenderingDevice();

		// works for VS
		StringBuilder builder;
		builder.Append(ProjectConfig->ProjectPath);
		builder.Append("/Build/");
		builder.Append(ProjectConfig->ProjectName);
		builder.Append("/Debug/AssetsPathConfig.json");

		gApp->EngineMgr.InitEngine(std::move(game), std::move(device), builder.GetString());
	}
	else
		gApp->EngineMgr.Edit();
}

void ProjectManager::Play()
{
	if (!ProjectConfig)
		throw new ProjectManagerException("This operation requires any project opened.");

	if (gApp->EngineMgr.GetEngineState() == eEngineState::NONE)
	{
		// load game
		if (!LoadGame.FunctionValid())
		{
			LoadGame = LoadFunctionFromSharedLibrary<CreateGameFunc>(ProjectConfig->GetGameDllPath().GetCStr(), "CreateGame");
			ASSERTE(LoadGame.FunctionValid(), "Library libGame load failed");
			gConsole.LogDebug("Library libGame loaded.");
		}

		std::unique_ptr<IGame> game = std::unique_ptr<IGame>(LoadGame());
		std::unique_ptr<IRenderingDevice> device = gApp->Ui.MainViewport->GetRenderingDevice();

		// works for VS
		StringBuilder builder;
		builder.Append(ProjectConfig->ProjectPath);
		builder.Append("/Build/");
		builder.Append(ProjectConfig->ProjectName);
		builder.Append("/Debug/AssetsPathConfig.json");

		gApp->EngineMgr.InitEngine(std::move(game), std::move(device), builder.GetString());
	}
	
	// TODO(squares): fix problem with physics; Rigidbody and collider components  are initialized in next frame 
	//		so when next frame never occur we try to delete empty ImplData
	gApp->EngineMgr.Play();
}

void ProjectManager::Close()
{
	if (!ProjectConfig)
		throw new ProjectManagerException("This operation requires any project opened.");

	ProjectConfig.release();
}