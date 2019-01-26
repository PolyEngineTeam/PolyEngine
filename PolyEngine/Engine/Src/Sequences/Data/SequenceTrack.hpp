#pragma once

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
			float StartTime;
			std::shared_ptr<IAction> Action;
		};

		explicit SequenceTrack(std::vector<RegisteredAction>&& actions);

		bool IsActive();

		void OnBegin(Entity* entity);
		void OnUpdate(float deltaTime);
		void OnAbort();

	private:
		void UpdateActiveActions(const float deltaTime);
		void TryActivateNextAction(const float deltaTime);

		float Time = 0;
		Entity* EntityObj = nullptr;

		std::vector<RegisteredAction> Actions;
		std::vector<IAction*> ActiveActions;
		std::vector<RegisteredAction>::iterator NextAction;

	}; // class SequenceTrack
} // namespace Poly