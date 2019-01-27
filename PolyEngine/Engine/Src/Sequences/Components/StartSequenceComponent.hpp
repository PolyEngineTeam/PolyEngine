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
		void AddSequenceToStart(String sequenceName);
		void AddSequencesToStart(const std::vector<String>& sequencesNames);

		const std::vector<String>& GetSequencesToStart() const;
	private:
		std::vector<String> Names;

	};

	REGISTER_COMPONENT(ComponentsIDGroup, StartSequenceComponent)
} // namespace Poly