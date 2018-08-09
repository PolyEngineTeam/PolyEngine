#pragma once

#include <QtCore/qobject.h>

#include <Engine.hpp>

class IEngineManager : public QObject
{
	Q_OBJECT;

public:
	virtual Poly::eEngineState GetEngineState() = 0;

	virtual void InitEngine(std::unique_ptr<Poly::IGame> game, const Poly::String& assetsPathConfigPath) = 0;
	virtual void DeinitEngine() = 0;

	virtual void Edit() = 0;
	virtual void Play() = 0;

signals:
	void EngineInitialized(Poly::Engine* engine);
	void EngineDeinitialized();
	void StateChanged(Poly::eEngineState state);
};