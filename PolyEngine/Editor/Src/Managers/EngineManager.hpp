#pragma once

#include <QObject>
#include <QTimer>
#include <Engine.hpp>

using namespace Poly;

enum class eEngineState
{
	EDIT,
	GAMEPLAY,
	NONE,
	_COUNT
};

class EngineManager : public QObject
{
	Q_OBJECT

public:
	EngineManager();

	eEngineState GetEngineState() { return EngineState; }

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
	eEngineState EngineState = eEngineState::NONE;

	QTimer Updater;

private slots:
	void UpdatePhase();
};