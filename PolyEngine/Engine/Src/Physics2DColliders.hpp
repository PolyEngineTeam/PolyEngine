#pragma once

#include "ComponentBase.hpp"

class b2PolygonShape;

namespace Poly
{
	class ENGINE_DLLEXPORT Box2DColliderComponent : public ComponentBase
	{
	public:
		Box2DColliderComponent(const Vector& size);
		~Box2DColliderComponent();

		void SetSize(Vector size);

		const b2PolygonShape* GetShape() const { return Shape.get(); }
	private:
		std::unique_ptr<b2PolygonShape> Shape;
	};


}