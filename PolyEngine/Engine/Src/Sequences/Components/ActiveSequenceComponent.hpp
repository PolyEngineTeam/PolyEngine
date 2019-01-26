#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>

namespace Poly
{
	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ActiveSequenceComponent : public ComponentBase
	{
		RTTI_DECLARE_COMPONENT(::Poly::ActiveSequenceComponent)
		{
			NO_RTTI_PROPERTY();
		}

	public:
		explicit ActiveSequenceComponent(String sequenceName);
		explicit ActiveSequenceComponent(Dynarray<String> sequencesNames);

		void AddActiveSequence(String sequenceName);
		void AddActiveSequence(const Dynarray<String>& sequencesNames);

		const Dynarray<String>& GetActiveSequencesNames() const;
	private:
		Dynarray<String> Names;

	}; // class ActiveSequenceComponent
} // namespace Poly