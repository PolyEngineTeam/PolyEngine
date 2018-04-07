#pragma once

#include <Core.hpp>
#include <QObject>

#include "Configs/ProjectConfig.hpp"

class ProjectManagerException : public std::exception
{
public:
	ProjectManagerException(const Poly::String& msg) : Msg(msg) {}
	const char* what() const noexcept override { return Msg.GetCStr(); }

protected:
	Poly::String Msg;
};

class ProjectManager : public QObject
{
public:
	ProjectManager() {}
	~ProjectManager() = default;

	// creates and opens project so the game is ready to be played (but you have to build it first)
	void Create(const Poly::String& projectName, const Poly::String& projectPath, const Poly::String& enginePath);
	// opens project so the game is playable (you might have to build it first)
	void Open(Poly::String projectPath);
	// updates project from given
	void Update(const Poly::String& projedtPath, const Poly::String& enginePath);
	void Update(const Poly::String& enginePath);
	void Build();
	void Close();

	bool IsOpened() { return Opened; }

	const Poly::String& GetProjectName() const { return ProjectConfig->ProjectName; }
	const Poly::String& GetProjectPath() const { return ProjectPath; }

private:
	bool Opened = false;

	Poly::String ProjectPath = "";
	std::unique_ptr<ProjectConfig> ProjectConfig;
};