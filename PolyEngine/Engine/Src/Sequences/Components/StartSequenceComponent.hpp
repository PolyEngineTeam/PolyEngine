#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>

namespace Poly
{
	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT StartSequenceComponent : public ComponentBase
	{
		RTTI_DECLARE_COMPONENT(::Poly::StartSequenceComponent)
		{
			NO_RTTI_PROPERTY();
		}

	public:
		explicit StartSequenceComponent(String sequenceName);
		explicit StartSequenceComponent(Dynarray<String> sequencesNames);

		void AddSequenceToStart(String sequenceName);
		void AddSequencesToStart(const Dynarray<String>& sequencesNames);

		const Dynarray<String>& GetSequencesToStart() const;
	private:
		Dynarray<String> Names;

	};

	REGISTER_COMPONENT(ComponentsIDGroup, StartSequenceComponent)
} // namespace Poly