#pragma once

#include <Core.hpp>
#include <QObject>
#include <QTimer>

#include "Configs/ProjectConfig.hpp"

using namespace Poly;

class ProjectManagerException : public std::exception
{
public:
	ProjectManagerException(const String& msg) : Msg(msg) {}
	const char* what() const noexcept override { return Msg.GetCStr(); }

protected:
	String Msg;
};

class ProjectManager : public QObject
{
	Q_OBJECT

public:
	ProjectManager() {}
	~ProjectManager() = default;

	// creates new project with ProjectTool.py
	void Create(const String& projectName, const String& projectPath, const String& enginePath);
	// loads ProjectConfig from *.proj.json file 
	void Open(String projectPath);
	// updates project from given engine
	void Update(const String& enginePath);
	// builds current project
	void Build();
	// go into edit mode
	void Edit();
	// run game in editor's viewport
	void Play(); 
	// closes current project
	void Close();

	bool IsOpened() { ProjectCfg; }

	const ProjectConfig& GetProjectConfig() const { return *ProjectCfg; }

	const String& SetProjectName(const String& name) const { return ProjectCfg->ProjectName = name; }

signals:
	void ProjectOpened(::ProjectConfig* config);
	void ProjectClosed();

private:
	std::unique_ptr<ProjectConfig> ProjectCfg;
};