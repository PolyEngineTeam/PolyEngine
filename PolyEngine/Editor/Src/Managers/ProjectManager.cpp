#include "PolyEditorPCH.hpp"

#include <Configs/AssetsPathConfig.hpp>
#include "Utils/LibraryLoader.hpp"

extern "C"
{
	using CreateGameFunc = Poly::IGame* (void);
}

static Poly::LibraryFunctionHandle<CreateGameFunc> LoadGame;

void ProjectManager::Create(const Poly::String& projectName, const Poly::String& projectPath, const Poly::String& enginePath)
{
	if (Opened)
		throw new ProjectManagerException("Can't create new project without closing previous one.");

	ProjectPath = projectPath;

	Poly::StringBuilder builder;

	builder.Append("py ");
	builder.Append(enginePath);
	builder.Append("/Scripts/ProjectTool.py -e ");
	builder.Append(enginePath);
	builder.Append(" -c ");
	builder.Append(ProjectPath);
	builder.Append(" ");
	builder.Append(projectName);

	gApp->CommandMgr.RunCommand(builder.GetString(), "Project creation");
}

void ProjectManager::Open(Poly::String projectPath)
{
	if (Opened)
		throw new ProjectManagerException("Can't open project without closing previous one.");

	ProjectConfig = std::make_unique<::ProjectConfig>(projectPath);
	ProjectConfig->Load();

	for (size_t i = projectPath.GetLength(); i > 0; --i)
		if (projectPath[i] == '/')
		{
			ProjectPath = projectPath.Substring(i);
			break;
		}

	// works for VS
	Poly::StringBuilder builder;
	builder.Append(ProjectPath);
	builder.Append("/Build/");
	builder.Append(ProjectConfig->ProjectName);
	builder.Append("/Debug/AssetsPathConfig.json");
	Poly::gAssetsPathConfig.DeserializeFromFile(builder.GetString());

	// load game
	if (!LoadGame.FunctionValid())
	{
		Poly::String str = Poly::gAssetsPathConfig.GetGameLibPath();
		str = Poly::gAssetsPathConfig.GetRenderingDeviceLibPath();
		str = Poly::gAssetsPathConfig.GetAssetsPath(Poly::eResourceSource::GAME);
		str = Poly::gAssetsPathConfig.GetAssetsPath(Poly::eResourceSource::ENGINE);

		LoadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>(Poly::gAssetsPathConfig.GetGameLibPath().GetCStr(), "CreateGame");
		ASSERTE(LoadGame.FunctionValid(), "Library libGame load failed");
		Poly::gConsole.LogDebug("Library libGame loaded.");
	}

	std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(LoadGame());
	std::unique_ptr<Poly::IRenderingDevice> device = gApp->Ui.MainViewport->InitializeViewport();

	gApp->GameMgr.Init(std::move(game), std::move(device));

	Opened = true;
}

void ProjectManager::Update(const Poly::String& enginePath)
{
	if (!Opened)
		throw new ProjectManagerException("This operation requires any project opened.");

	Poly::StringBuilder builder;

	builder.Append("py Z:/Programming/C++/PolyEngine/PolyEngine/Scripts/ProjectTool.py -e ");
	builder.Append(enginePath);
	builder.Append(" -u ");
	builder.Append(ProjectPath);

	gApp->CommandMgr.RunCommand(builder.GetString(), "Project update");
}

void ProjectManager::Build()
{
	if (!Opened)
		throw new ProjectManagerException("This operation requires any project opened.");

	if (Running)
		throw new ProjectManagerException("Another operation is currently running.");

	Poly::StringBuilder builder;

	builder.Append("cmake --build ");
	builder.Append(ProjectPath);
	builder.Append("/Build");

	Command = builder.GetString();
	CommandDesc = "Project build";

	RunCommand(Command);
}

void ProjectManager::Close()
{
	Opened = false;

	ProjectPath = "";
}