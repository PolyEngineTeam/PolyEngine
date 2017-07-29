#pragma once

namespace SGJ
{
	//TODO ADD MORE POWERUPS
	enum class ePowerup
	{
		NONE,				//Used as a starting state
		REVERSED_CONTROLS,	//Reversed X axis
		HIGH_JUMP,			//Increased jump height
		POGO_JUMP,			//Constantly jumping
		INCREASED_SPEED,
		INCREASED_SIZE,
		DECREASED_SIZE,
		INVERSED_GRAVITY
	};
}