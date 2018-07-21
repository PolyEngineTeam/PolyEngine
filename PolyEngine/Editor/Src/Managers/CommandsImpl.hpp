#pragma once

#include <Core.hpp>
#include <ECS/Entity.hpp>

#include "Managers/InspectorManager.hpp"

using namespace Poly;


struct PointerWrapper
{
	void* Ptr;
	size_t Sz;

	template<typename T>
	PointerWrapper(T* pointer)
	{
		Sz = sizeof(T);
		Ptr = pointer;
	}

	PointerWrapper()
	{
		Ptr = nullptr;
		Sz = 0;
	}

	~PointerWrapper()
	{
		operator delete(Ptr, Sz);
	}
};

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
class ControlCommand : public Command
{
public:
	~ControlCommand() {}

	void Undo() override
	{
		UndoPtr(this);
	}
	void Redo() override
	{
		RedoPtr(this);
	}

	void (*UndoPtr)(ControlCommand*);
	void (*RedoPtr)(ControlCommand*);

	void* Object;
	ControlBase* Control;

	PointerWrapper UndoValue;
	PointerWrapper RedoValue;
};