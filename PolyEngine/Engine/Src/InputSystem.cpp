#include "InputSystem.hpp"

#include "World.hpp"
#include "InputWorldComponent.hpp"

using namespace Poly;

void InputSystem::InputPhase(World* world)
{
	InputWorldComponent* com = world->GetInputWorldComponent();

	for (int i = 0; i < 256; i++)
		com->PrevKey[i] = com->CurrKey[i];

	//memcpy(&com->PrevKey, &com->CurrKey, 256 * sizeof(bool));

	com->PrevMouse = com->CurrMouse;
	com->PrevWheel = com->CurrWheel;

	InputQueue* InputEventsQueue = world->GetEngine()->GetInputQueue();

	while (!InputEventsQueue->Empty())
	{
		switch (InputEventsQueue->Front().Type)
		{
		case eInputEventType::KEYDOWN:
			com->CurrKey[static_cast<int>(InputEventsQueue->Front().Key)] = true;
			break;
		case eInputEventType::KEYUP:
			com->CurrKey[static_cast<int>(InputEventsQueue->Front().Key)] = false;
			break;
		case eInputEventType::MOUSEMOVE:
			com->CurrMouse = InputEventsQueue->Front().Pos;
			break;
		case eInputEventType::WHEELMOVE:
			com->CurrWheel += InputEventsQueue->Front().Pos.X;
			break;
		}
		InputEventsQueue->Pop();
	}

	/*while(InputEventsQueue->Size() > 0){
		if(InputEventsQueue->Front().Type == eInputEventType::KEYDOWN)
			gConsole.LogDebug("Keydown: {}", (unsigned)InputEventsQueue->Front().Key);
		else if(InputEventsQueue->Front().Type == eInputEventType::KEYUP)
			gConsole.LogDebug("Keyup: {}", (unsigned)InputEventsQueue->Front().Key);
		else if(InputEventsQueue->Front().Type == eInputEventType::MOUSEMOVE)
			gConsole.LogDebug("Mousemove: {}", InputEventsQueue->Front().Pos);
		else if(InputEventsQueue->Front().Type == eInputEventType::WHEELMOVE)
			gConsole.LogDebug("Wheelmoove: {}", InputEventsQueue->Front().Pos);
		InputEventsQueue->Pop();
	}*/

	if(com->GetPressed(eKey::ALT)) gConsole.LogDebug("ALT pressed!!");
	if(com->GetPressed(eKey::MLBUTTON)) gConsole.LogDebug("MLBUTTON pressed!!");
	if(com->GetPressed(eKey::SEMICOLON_COLON)) gConsole.LogDebug("SEMICOLON pressed!!");

	if(com->GetPressed(eKey::ALT, eKey::LSHIFT)) gConsole.LogDebug("ALT and LSHIFT pressed!!");
	if(com->GetPressed(eKey::MLBUTTON, eKey::MRBUTTON)) gConsole.LogDebug("MLBUTTON and MRBUTTON pressed!!");
	if(com->GetPressed(eKey::CTRL, eKey::KEY_4)) gConsole.LogDebug("CTRL and KEY_4 pressed!!");

	if(com->GetPressed(eKey::ALT, eKey::LSHIFT, eKey::KEY_H)) gConsole.LogDebug("ALT and LSHIFT and KEY_G pressed!!");
	if(com->GetPressed(eKey::MLBUTTON, eKey::MRBUTTON, eKey::MMBUTTON)) gConsole.LogDebug("MLBUTTON and MRBUTTON and MMBUTTON pressed!!");
	if(com->GetPressed(eKey::CTRL, eKey::KEY_4, eKey::KEY_3)) gConsole.LogDebug("CTRL and KEY_4 and KEY_3 pressed!!");

	if(com->GetClicked(eKey::KEY_0)) gConsole.LogDebug("KEY_0 clicked!!");
	if(com->GetClicked(eKey::KEY_A)) gConsole.LogDebug("KEY_A clicked!!");
	if(com->GetClicked(eKey::KEY_G)) gConsole.LogDebug("KEY_G clicked!!");
	if(com->GetClicked(eKey::SHIFT)) gConsole.LogDebug("SHIFT clicked!!");
	if(com->GetClicked(eKey::RSHIFT)) gConsole.LogDebug("RSHIFT clicked!!");
	if(com->GetClicked(eKey::ENTER)) gConsole.LogDebug("ENTER clicked!!");

	if(com->GetReleased(eKey::KEY_0)) gConsole.LogDebug("KEY_0 released!!");
	if(com->GetReleased(eKey::KEY_A)) gConsole.LogDebug("KEY_A released!!");
	if(com->GetReleased(eKey::KEY_G)) gConsole.LogDebug("KEY_G released!!");
	if(com->GetReleased(eKey::SHIFT)) gConsole.LogDebug("SHIFT released!!");
	if(com->GetReleased(eKey::RSHIFT)) gConsole.LogDebug("RSHIFT released!!");
	if(com->GetReleased(eKey::ENTER)) gConsole.LogDebug("ENTER released!!");
}

