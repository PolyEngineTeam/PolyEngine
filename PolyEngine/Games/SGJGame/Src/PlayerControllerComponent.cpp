#include "PlayerControllerComponent.hpp"

namespace SGJ
{
	PlayerControllerComponent::PlayerControllerComponent(float movementSpeed, float jumpForce) :
		DefJumpForce(jumpForce),
		MovementSpeed(movementSpeed),
		JumpForce(jumpForce)
	{
	}
}
