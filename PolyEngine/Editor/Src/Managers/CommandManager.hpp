#pragma once

#include <QtCore/qcoreevent.h>

#include <Defines.hpp>

using namespace Poly;

class Command;

class CommandManager
{
public:
	CommandManager();
	~CommandManager();

	void ProcessEvent(QEvent* event);

	void AddCommand(Command* cmd);

private:
	void Undo();
	void Redo();

	Dynarray<Command*> Commands;
	size_t CurrentCommand = 0;

	bool CtrlPressed = false;
	bool ShiftPressed = false;
	bool ZPressed = false;
	bool DuringEvent = false;
};