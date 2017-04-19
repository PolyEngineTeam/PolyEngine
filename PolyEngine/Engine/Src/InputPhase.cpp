#include "InputPhase.hpp"

using namespace Poly;

void InputPhase::Input(World* world)
{
	InputWorldComponent* com = nullptr;//world->GetComponent(InputWorldComponentID);


	for (int i = 0; i < 256; i++)
		com->PrevKey[i] = com->CurrKey[i];

	com->PrevMouse = com->CurrMouse;
	com->PrevWheel = com->CurrWheel;

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
			com->CurrWheel = InputEventsQueue->Front().Pos;
			break;
		}
		InputEventsQueue->Pop();
	}
}

