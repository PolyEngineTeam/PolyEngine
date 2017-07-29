#pragma once

#include "ComponentBase.hpp"
#include "CameraSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT PostprocessSettingsComponent : public ComponentBase
	{
		friend void CameraSystem::CameraUpdatePhase(World*);
	public:

		int UseCashetes = 0;
		float ColorTempValue = 20000.0f; // 6500.0 from [1000.0, 40000.0]
		float ColorTempPower = 1.0f;
		float ColorTempLuminancePreservation = 0.75f;
		float AberationPower = 0.55f;
		float GrainPower = 0.1f;
		float StripesPower = 0.55f;
		float VinettePower = 0.60f;

	private:
	};
}