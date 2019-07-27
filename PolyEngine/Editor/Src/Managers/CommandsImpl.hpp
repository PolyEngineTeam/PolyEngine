#pragma once

#include <Defines.hpp>
#include <ECS/Entity.hpp>
#include "Managers/Project/InspectorManager.hpp"

using namespace Poly;

template <typename T> class ControlBase;

//------------------------------------------------------------------------------
class Command
{
public:
	virtual ~Command() = default;

	virtual void Undo() = 0;
	virtual void Redo() = 0;
};

//------------------------------------------------------------------------------
class VoidCommand : public Command
{
public:
	void Undo() override {}
	void Redo() override {}
};

//------------------------------------------------------------------------------
class EntitiesSelectionChangedCommand : public Command
{
public:
	void Undo() override
	{
		Manager->SelectedEntities = OldEntities;
		Manager->EntitiesSelectionChanged();
	}
	void Redo() override
	{
		Manager->SelectedEntities = NewEntities;
		Manager->EntitiesSelectionChanged();
	}

	std::vector<Entity*> OldEntities;
	std::vector<Entity*> NewEntities;
	InspectorManager* Manager;
};

//------------------------------------------------------------------------------
template <typename T>
class ControlCommand : public Command
{
public:
	virtual ~ControlCommand() 
	{
		delete UndoValue;
		delete RedoValue;
	}

	void Undo() override
	{
		*Object = *UndoValue;
	}

	void Redo() override
	{
		*Object = *RedoValue;
	}

	T* Object;
	ControlBase<T>* Control;
	InspectorManager* Manager;

	T* UndoValue;
	T* RedoValue;
};