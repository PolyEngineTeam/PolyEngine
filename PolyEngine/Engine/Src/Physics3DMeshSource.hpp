#pragma once

#include <BaseObject.hpp>
#include <Vector.hpp>

namespace Poly
{
	struct Physics3DMeshSourceImplData;

	class ENGINE_DLLEXPORT Physics3DMeshSource : public BaseObject<>
	{
		friend class Physics3DMeshShape;

	public:
		Physics3DMeshSource();
		~Physics3DMeshSource();

		void AddTriangle(const Vector& a, const Vector& b, const Vector& c);

	private:
		std::unique_ptr<Physics3DMeshSourceImplData> ImplData;
	};
} // namepsace Poly