#pragma once

#include <QObject>
#include <QTimer>
#include <Engine.hpp>

using namespace Poly;

class EngineManager : public QObject
{
	Q_OBJECT

public:
	EngineManager();

	eEngineState GetEngineState() { return Editor ? Editor->GetEngineState() : eEngineState::NONE; }

	void InitEngine(std::unique_ptr<IGame> game, const String& assetsPathConfigPath);
	void DeinitEngine();

	void Edit();
	void Play();

signals:
	void Initialized(World* world);
	void StateChanged(eEngineState state);
	void Deinitialized();

private:
	std::unique_ptr<Engine> Engine = nullptr;
	IEditor* Editor = nullptr;

	QTimer Updater;

private slots:
	void UpdatePhase();
};