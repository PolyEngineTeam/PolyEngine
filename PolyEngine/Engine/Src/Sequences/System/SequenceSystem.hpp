#pragma once

#include <Defines.hpp>
#include <ECS/ISystem.hpp>

namespace Poly
{
	class Scene;
	class Entity;

	//------------------------------------------------------------------------------
	class SequenceSystem final : public ISystem
	{
	public:
		void OnUpdate(Scene* scene) final;

		void StartSequence(Entity* entity, String name);
		// TODO(squares): implement this
		//void ResumeSequence(Entity* entity, String name);
		//void PauseSequence(Entity* entity, String name);
		//void StopSequence(Entity* entity, String name);
		//void ResmeAllSequences(Entity* entity);
		//void PauseAllSequences(Entity* entity);
		//void StopAllSequences(Entity* entity);
		
	}; // class SequenceSystem
} // namespace Poly