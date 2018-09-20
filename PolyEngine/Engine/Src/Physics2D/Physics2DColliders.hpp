#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>

class b2PolygonShape;
class b2CircleShape;

namespace Poly
{
	class ENGINE_DLLEXPORT Box2DColliderComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::Box2DColliderComponent) { NO_RTTI_PROPERTY(); }

		Box2DColliderComponent(const Vector& size);
		~Box2DColliderComponent();

		void SetSize(const Vector& size);

		const b2PolygonShape* GetShape() const { return Shape.get(); }
	private:
		std::unique_ptr<b2PolygonShape> Shape;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, Box2DColliderComponent)

	class ENGINE_DLLEXPORT Circle2DColliderComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::Circle2DColliderComponent) { NO_RTTI_PROPERTY(); }

		Circle2DColliderComponent(float size);
		~Circle2DColliderComponent();

		void SetSize(float size);

		const b2CircleShape* GetShape() const { return Shape.get(); }
	private:
		std::unique_ptr<b2CircleShape> Shape;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, Circle2DColliderComponent)
}