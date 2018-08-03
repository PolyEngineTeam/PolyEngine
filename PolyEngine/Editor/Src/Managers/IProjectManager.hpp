#pragma once

#include <QtCore/qobject.h>

#include <Core.hpp>

#include "Configs/ProjectConfig.hpp"
#include "Managers/IEngineManager.hpp"

class IProjectManager : public QObject
{
	Q_OBJECT

public:
	IProjectManager(std::unique_ptr<IEngineManager> engineManager)
		: EngineManager(std::move(engineManager))
	{
	}

	// Creates new project.
	virtual void Create(const Poly::String& projectName, const Poly::String& projectPath, const Poly::String& enginePath) const = 0;
	// Updates project from given engine.
	virtual void Update(const Poly::String& projectName, const Poly::String& projectPath, const Poly::String& enginePath) = 0;
	// Loads ProjectConfig from *.proj.json file.
	virtual void Open(const Poly::String& projectPath) = 0;
	// Closes current project.
	virtual void Close() = 0;

	// Ordinary save (saves only scene json file) does not update dlls and redistributeblaes.
	virtual void Save() = 0;
	// Save current project in given directory with given name (saves project json and copies game sources).
	virtual void SaveAs(const Poly::String& path, const Poly::String& Name) = 0;
	// Save current project in given directory with given name (saves project json and copies game dll).
	// Additionally copies all engine dlls, polyStandalone and all redistributables.
	virtual void SaveAsRelease(const Poly::String& path, const Poly::String& Name) = 0;

	// Edit mode.
	virtual void Edit() = 0;
	// Gameplay mode.
	virtual void Play() = 0;
	// Release mode.
	virtual void RunStandalone() = 0;

	// Returns true if any project is opened.
	virtual bool IsAnyProjectOpened() const = 0;
	// Returns project config loaded while opening project.
	virtual const ProjectConfig& GetProjectConfig() const = 0;

signals:
	void ProjectOpened(const ProjectConfig& config);
	void ProjectClosed();

protected:
	std::unique_ptr<IEngineManager> EngineManager
};