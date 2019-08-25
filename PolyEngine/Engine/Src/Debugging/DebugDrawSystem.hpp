#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/Vector.hpp>
#include <pe/core/math/AABox.hpp>
#include <pe/core/math/Frustum.hpp>

namespace Poly
{
	class Scene;

	namespace DebugDrawSystem
	{
		void ENGINE_DLLEXPORT DebugRenderingUpdatePhase(Scene*);

		/// Emitters of elementary shapes
		void ENGINE_DLLEXPORT DrawLine(Scene* world, const core::math::Vector& begin, const core::math::Vector& end, const core::math::Color& color = core::math::Color::WHITE);
		void ENGINE_DLLEXPORT DrawBox(Scene* world, const core::math::Vector& mins, const core::math::Vector& maxs, const core::math::Color& color = core::math::Color::WHITE);
		void ENGINE_DLLEXPORT DrawBox(Scene* world, const core::math::Vector& mins, const core::math::Vector& maxs, const core::math::Matrix& worldFromSpace, const core::math::Color& color = core::math::Color::WHITE);
		void ENGINE_DLLEXPORT DrawBox(Scene* world, const core::math::AABox& box, const core::math::Color& color = core::math::Color::WHITE);
		
		void ENGINE_DLLEXPORT DrawFrustum(Scene* scene, const core::math::Frustum& frustum, const core::math::Matrix& viewFromWorld = core::math::Matrix(), core::math::Color color = core::math::Color::WHITE);
		void ENGINE_DLLEXPORT DrawFrustumPoints(Scene* scene, const std::vector<core::math::Vector>& cornersInWorld, const core::math::Color color = core::math::Color::WHITE);

		/// Think of orientation as of normal vector
		void ENGINE_DLLEXPORT DrawCircle(Scene* world, const core::math::Vector& position, float radius, core::math::Vector orientation, const core::math::Color& color = core::math::Color::WHITE);
		void ENGINE_DLLEXPORT DrawSphere(Scene* world, const core::math::Vector& position, float radius, const core::math::Color& color = core::math::Color::WHITE);
		void ENGINE_DLLEXPORT DrawArrow(Scene* world, core::math::Vector position, core::math::Vector directionVector, const core::math::Color& color = core::math::Color::WHITE);

		void ENGINE_DLLEXPORT DrawText2D(Scene* world, const core::math::Vector2i& screenPosition, core::storage::String text, size_t fontSize = 16, const core::math::Color& color = core::math::Color::WHITE);
	}
}