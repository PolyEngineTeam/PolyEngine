#pragma once

#include <Core.hpp>


class ProjectManagerException : public std::exception
{
public:
	ProjectManagerException(const Poly::String& msg) : Msg(msg) {}
	const char* what() const noexcept override { return Msg.GetCStr(); }

protected:
	Poly::String Msg;
};

class ProjectManager
{
public:
	ProjectManager() {}
	~ProjectManager() = default;

	bool IsOpened() { return Opened; }

	void Create(const Poly::String& projectName, const Poly::String& projectPath, const Poly::String& enginePath);
	void Open(const Poly::String& projectPath);
	void Update();
	void Update(const Poly::String& enginePath);
	void Build();
	void StandaloneRun();
	void EditorRun();
	void Close();

	const Poly::String& GetProjectName() const { return Name; }
	const Poly::String& GetProjectPath() const { return ProjectPath; }
	const Poly::String& GetEnginePath() const { return EnginePath; }

private:
	bool Opened = false;
	Poly::String Name = "";
	Poly::String ProjectPath = "";
	Poly::String EnginePath = "";
};