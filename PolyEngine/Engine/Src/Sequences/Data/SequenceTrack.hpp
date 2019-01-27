#pragma once

#include <optional>

#include <Defines.hpp>
#include <RTTI/RTTI.hpp>

#include <Sequences/Data/IAction.hpp>

namespace Poly
{
	class Entity;

	class ENGINE_DLLEXPORT SequenceTrack
	{
	public:
		void AppendAction(TimeDuration startTime, std::shared_ptr<IAction> action);

		bool IsActive();

		void OnBegin(Entity* entity);
		void OnUpdate(TimeDuration deltaTime);
		void OnAbort();

	private:
		TimeDuration TryUpdateActiveAction(TimeDuration deltaTime);
		void TryFinishActiveAction(TimeDuration deltaTime);
		bool TryStartNextAction();
		void TryFinishTrack();

		struct RegisteredAction
		{
			TimeDuration StartTime;
			std::shared_ptr<IAction> Action;
		};

		Entity* EntityObj = nullptr;
		TimeDuration TrackElapsedTime;

		std::vector<RegisteredAction> Actions;
		size_t NextActionIndex = 0;
		IAction* ActiveAction = nullptr;
		bool Active = false;

	}; // class SequenceTrack
} // namespace Poly