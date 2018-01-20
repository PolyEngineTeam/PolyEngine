#pragma once

#include <initializer_list>
#include <Vector2i.hpp>
#include <EnumUtils.hpp>
#include "ComponentBase.hpp"
#include "KeyBindings.hpp"
#include "InputSystem.hpp"

namespace Poly
{
    struct Controller {
        Controller() = default;

        EnumArray<bool, eControllerButton> CurrButton;
        EnumArray<bool, eControllerButton> PrevButton;
        EnumArray<float, eControllerAxis> CurrAxis;
        EnumArray<float, eControllerAxis> PrevAxis;
    };

	/// <summary>World component that holds input data.</summary>
	class ENGINE_DLLEXPORT InputWorldComponent : public ComponentBase
	{
	friend void InputSystem::InputPhase(World*);
	public:
		InputWorldComponent() = default;

		bool IsPressed(eKey key) const { return CurrKey[key]; }
		bool IsPressed(const std::initializer_list<eKey>& list) const;
		bool IsClicked(eKey key) const { return (CurrKey[key] && !PrevKey[key]); }
		bool IsReleased(eKey key) const { return (!CurrKey[key] && PrevKey[key]); }

		bool IsPressed(eMouseButton button) const { return CurrMouseButton[button]; }
		bool IsPressed(const std::initializer_list<eMouseButton>& list) const;
		bool IsClicked(eMouseButton button) const { return (CurrMouseButton[button] && !PrevMouseButton[button]); }
		bool IsReleased(eMouseButton button) const { return (!CurrMouseButton[button] && PrevMouseButton[button]); }

        bool IsPressed(size_t controllerID, eControllerButton button) const
        {
            if(ControllerPointers.GetSize() <= controllerID)
            {
                return false;
            }
            SDL_GameController* controller = ControllerPointers[controllerID];
            return Controllers.at(controller).CurrButton[button];
        }

		const Vector2i& GetMousePos() const { return CurrMouse; }
		Vector2i GetMousePosDelta() const  { return CurrMouse - PrevMouse; }

		const Vector2i& GetWheelPos() const { return CurrWheel; }
		Vector2i GetWheelPosDelta() const { return CurrWheel - PrevWheel; }

        float GetControllerAxis(size_t controllerID, eControllerAxis axis) const
        {
            if(ControllerPointers.GetSize() <= controllerID)
            {
                return 0.0f;
            }
            SDL_GameController* controller = ControllerPointers[controllerID];
            return Controllers.at(controller).CurrAxis[axis];
        }
	private:
		EnumArray<bool, eKey> CurrKey;
		EnumArray<bool, eKey> PrevKey;
		EnumArray<bool, eMouseButton> CurrMouseButton;
		EnumArray<bool, eMouseButton> PrevMouseButton;
		Vector2i CurrMouse;
		Vector2i PrevMouse;
		Vector2i CurrWheel;
		Vector2i PrevWheel;
        std::unordered_map<SDL_GameController*, Controller> Controllers;
        Dynarray<SDL_GameController*> ControllerPointers;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, InputWorldComponent)
}