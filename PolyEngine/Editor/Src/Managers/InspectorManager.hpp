#pragma once

#include <qobject.h>

#include "Managers/EngineManager.hpp"
#include "Configs/ProjectConfig.hpp"

class InspectorManager : public QObject
{
	Q_OBJECT

public:
	InspectorManager();

signals:
	void EngineInitialized();
	void EngineDeinitialized();
	void StateChanged(eEngineState state);

private slots:
	void EngineInitializedSlot();
	void EngineDeinitializedSlot();
	void StateChangedSlot(eEngineState state);
};