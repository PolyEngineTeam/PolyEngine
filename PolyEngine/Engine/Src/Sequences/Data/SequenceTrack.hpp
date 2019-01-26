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
		struct RegisteredAction
		{
			TimeDuration StartTime;
			std::shared_ptr<IAction> Action;
		};

		void AddAction(RegisteredAction action);

		bool IsActive();

		void OnBegin(Entity* entity);
		void OnUpdate(TimeDuration deltaTime);
		void OnAbort();

	private:
		Entity* EntityObj = nullptr;
		TimePoint TrackStartTime;

		std::vector<RegisteredAction> Actions;
		size_t NextActionIndex = 0;
		IAction* ActiveAction = nullptr;

	}; // class SequenceTrack
} // namespace Poly