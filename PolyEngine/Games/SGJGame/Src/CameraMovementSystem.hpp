#pragma once

namespace Poly { class World; }

namespace SGJ
{
	namespace CameraMovementSystem
	{
		void CameraMovementUpdatePhase(Poly::World* world);

		float ElasticEaseOut(float p);
	}
}