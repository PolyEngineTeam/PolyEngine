#pragma once

#include <Core.hpp>
#include <ECS/Entity.hpp>

#include "Managers/InspectorManager.hpp"

using namespace Poly;

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

	Dynarray<Entity*> OldEntities;
	Dynarray<Entity*> NewEntities;
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
		*reinterpret_cast<T*>(Object) = *UndoValue;
		emit Control->ObjectUpdated(this);
	}

	void Redo() override
	{
		*reinterpret_cast<T*>(Object) = *RedoValue;
		emit Control->ObjectUpdated(this);
	}

	void* Object;
	ControlBase* Control;

	T* UndoValue;
	T* RedoValue;
};