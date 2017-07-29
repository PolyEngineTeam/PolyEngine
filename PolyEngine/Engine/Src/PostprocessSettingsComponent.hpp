#pragma once

#include "ComponentBase.hpp"
#include "CameraSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT PostprocessSettingsComponent : public ComponentBase
	{
		friend void CameraSystem::CameraUpdatePhase(World*);
	public:

		bool UseBgShader = false;
		bool UseFgShader = false;

		int UseCashetes = 0;
		float AberationPower = 0.35f;
		float ColorTempValue = 10000.0f; // 6500.0 from [1000.0, 40000.0]
		float ColorTempPower = 1.0f;
		float ColorTempLuminancePreservation = 0.75f;
		float SaturationPower = 1.0f;
		float GrainPower = 0.1f;
		float StripesPower = 0.15f;
		float VinettePower = 0.20f;

	private:
	};
}