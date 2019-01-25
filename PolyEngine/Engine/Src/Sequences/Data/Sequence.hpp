#pragma once

#include <Defines.hpp>
#include <RTTI/RTTI.hpp>

namespace Poly
{
	class Sequence : public RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(::Poly::Sequence, ::Poly::RTTIBase)
		{
			NO_RTTI_PROPERTY();
		}

	public:
		virtual String GetName() const = 0;

		virtual void OnUpdate(float dt) = 0;

		virtual void OnBegin() = 0;
		virtual void OnStop() = 0;
		virtual void OnFinish() = 0;

	}; // class Sequence
} // namespace Poly