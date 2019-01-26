#pragma once

#include <optional>

#include <Defines.hpp>
#include <RTTI/RTTI.hpp>

#include <Sequences/Data/IAction.hpp>

namespace Poly
{
	class Entity;

	class SequenceTrack : public RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(::Poly::SequenceTrack, ::Poly::RTTIBase)
		{
			NO_RTTI_PROPERTY();
		}

	public:
		struct RegisteredAction
		{
			TimePoint StartTime;
			std::shared_ptr<IAction> Action;
		};

		explicit SequenceTrack(std::vector<RegisteredAction>&& actions);

		bool IsActive();

		void OnBegin(Entity* entity);
		void OnUpdate(TimeDuration deltaTime);
		void OnAbort();

	private:
		Entity* EntityObj = nullptr;

		std::vector<RegisteredAction> Actions;
		size_t NextActionIndex = 0;
		IAction* ActiveAction = nullptr;

	}; // class SequenceTrack
} // namespace Poly