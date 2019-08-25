#pragma once

#include <pe/Defines.hpp>
#include <QtCore/qobject.h>
#include <QtCore/qtimer.h>

#include "Configs/ProjectConfig.hpp"

using namespace Poly;

class ProjectManagerException : public std::exception
{
public:
	ProjectManagerException(const ::pe::core::storage::String& msg) : Msg((const char*)msg.GetCStr()) {}
	const char* what() const noexcept override { return Msg.GetCStr(); }

protected:
	::pe::core::storage::String Msg;
};

class ProjectManager : public QObject
{
	Q_OBJECT

public:
	ProjectManager(std::unique_ptr<ProjectConfig> projectCfg);

	// go into edit mode
	void Edit();
	// run game in editor's viewport
	void Play(); 

	bool IsOpened() { return Opened; }

	const ProjectConfig& GetProjectConfig() const { return *ProjectCfg; }

signals:
	void EngineInitialized(Engine* engine);
	void EngineDeinitialized();
	void EngineStateChanged(PolyEditor::eEngineState state);

private:
	void InitEngine();

	bool Opened = false;
	std::unique_ptr<ProjectConfig> ProjectCfg;
};