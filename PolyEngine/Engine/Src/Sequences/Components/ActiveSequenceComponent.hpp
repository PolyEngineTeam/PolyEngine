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
		explicit ActiveSequenceComponent(std::vector<String> sequencesNames);

		void AddActiveSequence(String sequenceName);
		void AddActiveSequences(const std::vector<String>& sequencesNames);

		void RemoveActiveSequence(String sequenceName);
		void RemoveActiveSequences(const std::vector<String>& sequencesNames);

		const std::vector<String>& GetActiveSequencesNames() const;
	private:
		std::vector<String> Names;

	};

	REGISTER_COMPONENT(ComponentsIDGroup, ActiveSequenceComponent)
} // namespace Poly