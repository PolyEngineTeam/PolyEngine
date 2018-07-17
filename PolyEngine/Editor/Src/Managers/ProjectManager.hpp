#pragma once

#include <Core.hpp>
#include <QtCore/qobject.h>
#include <QtCore/qtimer.h>

#include "Configs/ProjectConfig.hpp"

using namespace Poly;

class ProjectManagerException : public std::exception
{
public:
	ProjectManagerException(const String& msg) : Msg((const char*)msg.GetCStr()) {}
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
	void Open(const String& projectPath);
	// updates project from given engine
	void Update(const String& enginePath);
	// ordinary save
	void Save();
	// save current project in given directory with given name
	void SaveAs(const String& path, const String& Name);
	// builds current project
	void Build();
	// go into edit mode
	void Edit();
	// run game in editor's viewport
	void Play(); 
	// closes current project
	void Close();

	bool IsOpened() { return Opened; }

	const ProjectConfig& GetProjectConfig() const { return *ProjectCfg; }

	const String& SetProjectName(const String& name) const { return ProjectCfg->ProjectName = name; }

signals:
	void ProjectOpened(ProjectConfig* config);
	void ProjectClosed();

private:
	void InitEngine();

	bool Opened = false;
	std::unique_ptr<ProjectConfig> ProjectCfg;
};