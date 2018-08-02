#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

#include <QKeyEvent>

//------------------------------------------------------------------------------
CommandManager::CommandManager()
{
	Commands.PushBack(new VoidCommand());
}

//------------------------------------------------------------------------------
CommandManager::~CommandManager()
{
	for (Command* cmd : Commands)
		delete cmd;

	Commands.Clear();
}

//------------------------------------------------------------------------------
void CommandManager::ProcessEvent(QEvent* event)
{
	if (event->type() == QEvent::Type::KeyPress)
	{
		if (((QKeyEvent*)event)->isAutoRepeat())
			((QKeyEvent*)event)->ignore();
		else
		{
			switch (((QKeyEvent*)event)->key())
			{
			case Qt::Key::Key_Control:
				CtrlPressed = true;
				break;

			case Qt::Key::Key_Shift:
				ShiftPressed = true;
				break;

			case Qt::Key::Key_Z:
				ZPressed = true;
				break;
			}
		}
		if (CtrlPressed && ShiftPressed && ZPressed && !DuringEvent)
		{
			DuringEvent = true;
			Redo();
		}
		else if (CtrlPressed && ZPressed && !DuringEvent)
		{
			DuringEvent = true;
			Undo();
		}
	}
	else if (event->type() == QEvent::Type::KeyRelease)
	{
		if (((QKeyEvent*)event)->isAutoRepeat())
			((QKeyEvent*)event)->ignore();
		else
		{
			switch (((QKeyEvent*)event)->key())
			{
			case Qt::Key::Key_Control:
				CtrlPressed = false;
				DuringEvent = false;
				break;

			case Qt::Key::Key_Shift:
				ShiftPressed = false;
				DuringEvent = false;
				break;

			case Qt::Key::Key_Z:
				ZPressed = false;
				DuringEvent = false;
				break;
			}
		}
	}
}

//------------------------------------------------------------------------------
void CommandManager::AddCommand(Command* cmd)
{
	if (Commands[CurrentCommand] == cmd)
		return;

	for (int i = (int)CurrentCommand + 1; i < (int)Commands.GetSize();)
	{
		delete Commands[Commands.GetSize() - 1];
		Commands.PopBack();
	}

	Commands.PushBack(cmd);

	++CurrentCommand;
}

//------------------------------------------------------------------------------
void CommandManager::Undo()
{
	gConsole.LogDebug("Undo: {}", CurrentCommand);

	if (CurrentCommand == 0)
		return;

	Commands[CurrentCommand]->Undo();
	--CurrentCommand;
}

//------------------------------------------------------------------------------
void CommandManager::Redo()
{
	if (CurrentCommand == Commands.GetSize() - 1)
		return;

	++CurrentCommand;
	Commands[CurrentCommand]->Redo(); 
	gApp->InspectorMgr->Update();
}
