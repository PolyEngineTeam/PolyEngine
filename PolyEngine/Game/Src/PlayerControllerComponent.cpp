#include "PlayerControllerComponent.hpp"

namespace SGJ
{
	PlayerControllerComponent::PlayerControllerComponent(float movementSpeed, float jumpForce) :
		MovementSpeed(movementSpeed),
		JumpForce(jumpForce),
		DefJumpForce(jumpForce)
	{
	}
}
