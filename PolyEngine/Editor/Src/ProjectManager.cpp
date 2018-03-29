#include "PolyEditorPCH.hpp"

void ProjectManager::Create(const Poly::String& projectName, const Poly::String& projectPath, const Poly::String& enginePath)
{
	if (Opened)
		throw new ProjectManagerException("Can't create new project without closing previous one.");

	Name = projectName;
	ProjectPath = projectPath;
	EnginePath = enginePath;

	Poly::StringBuilder builder;

	builder.Append("py Z:/Programming/C++/PolyEngine/PolyEngine/Scripts/ProjectTool.py ");
	builder.Append(" -e ");
	builder.Append(EnginePath);
	builder.Append(" -c ");
	builder.Append(ProjectPath);
	builder.Append(" ");
	builder.Append(Name);
	Command = builder.GetString();
	CommandDesc = "Project creation";

	RunCommand(Command);

	Opened = true;
}

void ProjectManager::Open(const Poly::String& projectPath)
{
	if (Opened)
		throw new ProjectManagerException("Can't open project without closing previous one.");

	// TODO (squares): get projectName from json
	// Name = projectName;
	ProjectPath = projectPath;

	Opened = true;
}

void ProjectManager::Update()
{
	if (!Opened)
		throw new ProjectManagerException("This operation requires any project opened.");

	Poly::StringBuilder builder;

	builder.Append("py Z:/Programming/C++/PolyEngine/PolyEngine/Scripts/ProjectTool.py -e ");
	builder.Append(EnginePath);
	builder.Append(" -u ");
	builder.Append(ProjectPath);

	system(builder.GetString().GetCStr());
}

void ProjectManager::Update(const Poly::String& enginePath)
{
	if (!Opened)
		throw new ProjectManagerException("This operation requires any project opened.");

	EnginePath = enginePath;

	Poly::StringBuilder builder;

	builder.Append("py Z:/Programming/C++/PolyEngine/PolyEngine/Scripts/ProjectTool.py -e ");
	builder.Append(EnginePath);
	builder.Append(" -u ");
	builder.Append(ProjectPath);

	system(builder.GetString().GetCStr());
}

void ProjectManager::Build()
{
	if (!Opened)
		throw new ProjectManagerException("This operation requires any project opened.");

	Poly::StringBuilder builder;

	builder.Append("cmake --build ");
	builder.Append(ProjectPath);
	builder.Append("/Build");

	system(builder.GetString().GetCStr());
}

void ProjectManager::StandaloneRun()
{

}

void ProjectManager::Close()
{
	Opened = false;

	Name = "";
	ProjectPath = "";
	EnginePath = "";
}

void ProjectManager::RunCommand(const Poly::String& cmd)
{
	if (Running)
		throw new ProjectManagerException("Another process is currently running within ProjectManager.");

	Poly::StringBuilder builder;

	builder.Append(cmd);
	builder.Append(" 2>&1");

	Stream = _popen(builder.GetString().GetCStr(), "r");

	Running = true;
	
	Timer = std::make_unique<QTimer>(this);
	connect(Timer.get(), &QTimer::timeout, this, &ProjectManager::ReadStdout);
	Timer->start(200);

	*Ostream << "\n\n> ----------     " << CommandDesc << " started...     ----------\n";
	*Ostream << "> ----------     " << Command << "     ----------\n\n";

}

void ProjectManager::ReadStdout()
{
	if (Stream && !feof(Stream))
	{
		if (fgets(Buffer, MaxBuffer, Stream) != NULL)
			*Ostream << "> " << Buffer;
	}
	else
	{
		_pclose(Stream);
		Running = false;
		*Ostream << "\n> ----------     " << CommandDesc << " finished.     ----------\n\n";
		Command = "";
		CommandDesc = "";
	}
}
