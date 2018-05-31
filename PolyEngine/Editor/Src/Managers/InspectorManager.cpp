#include "InspectorManager.hpp"

InspectorManager::InspectorManager()
{
}

void InspectorManager::EngineInitializedSlot()
{
	emit EngineInitialized();
}

void InspectorManager::EngineDeinitializedSlot()
{
	emit EngineDeinitialized();
}

void InspectorManager::StateChangedSlot(eEngineState state)
{
	emit StateChanged(state);
}
