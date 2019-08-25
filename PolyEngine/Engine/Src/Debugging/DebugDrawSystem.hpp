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
		void ENGINE_DLLEXPORT DrawLine(Scene* world, const ::pe::core::math::Vector& begin, const ::pe::core::math::Vector& end, const ::pe::core::math::Color& color = ::pe::core::math::Color::WHITE);
		void ENGINE_DLLEXPORT DrawBox(Scene* world, const ::pe::core::math::Vector& mins, const ::pe::core::math::Vector& maxs, const ::pe::core::math::Color& color = ::pe::core::math::Color::WHITE);
		void ENGINE_DLLEXPORT DrawBox(Scene* world, const ::pe::core::math::Vector& mins, const ::pe::core::math::Vector& maxs, const ::pe::core::math::Matrix& worldFromSpace, const ::pe::core::math::Color& color = ::pe::core::math::Color::WHITE);
		void ENGINE_DLLEXPORT DrawBox(Scene* world, const ::pe::core::math::AABox& box, const ::pe::core::math::Color& color = ::pe::core::math::Color::WHITE);
		
		void ENGINE_DLLEXPORT DrawFrustum(Scene* scene, const ::pe::core::math::Frustum& frustum, const ::pe::core::math::Matrix& viewFromWorld = ::pe::core::math::Matrix(), ::pe::core::math::Color color = ::pe::core::math::Color::WHITE);
		void ENGINE_DLLEXPORT DrawFrustumPoints(Scene* scene, const std::vector<::pe::core::math::Vector>& cornersInWorld, const ::pe::core::math::Color color = ::pe::core::math::Color::WHITE);

		/// Think of orientation as of normal vector
		void ENGINE_DLLEXPORT DrawCircle(Scene* world, const ::pe::core::math::Vector& position, float radius, ::pe::core::math::Vector orientation, const ::pe::core::math::Color& color = ::pe::core::math::Color::WHITE);
		void ENGINE_DLLEXPORT DrawSphere(Scene* world, const ::pe::core::math::Vector& position, float radius, const ::pe::core::math::Color& color = ::pe::core::math::Color::WHITE);
		void ENGINE_DLLEXPORT DrawArrow(Scene* world, ::pe::core::math::Vector position, ::pe::core::math::Vector directionVector, const ::pe::core::math::Color& color = ::pe::core::math::Color::WHITE);

		void ENGINE_DLLEXPORT DrawText2D(Scene* world, const ::pe::core::math::Vector2i& screenPosition, ::pe::core::storage::String text, size_t fontSize = 16, const ::pe::core::math::Color& color = ::pe::core::math::Color::WHITE);
	}
}