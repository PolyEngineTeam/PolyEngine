#pragma once

#include <Engine.hpp>
#include <QtCore/qobject.h>
#include <QtCore/qtimer.h>
#include "IEditor.hpp"

using namespace Poly;

class EngineManager : public QObject
{
	Q_OBJECT

public:
	EngineManager();

	PolyEditor::eEngineState GetEngineState() { return Editor ? Editor->GetEngineState() : PolyEditor::eEngineState::NONE; }

	void InitEngine(std::unique_ptr<IGame> game, const String& assetsPathConfigPath);
	void DeinitEngine();

	void Edit();
	void Play();

signals:
	void Created(Engine* engine);
	void Initialized();
	void Deinitialized();
	void StateChanged(PolyEditor::eEngineState state);

private:
	std::unique_ptr<Engine> EngineObj;
	PolyEditor::IEditor* Editor = nullptr;

	QTimer Updater;
	QTimer EditorUpdater;

private slots:
	void UpdatePhase();
	void EditorUpdatePhase();
};