#pragma once

#include <Defines.hpp>
#include <Sequences/Data/IAction.hpp>

namespace Poly
{
    class Scene;
    using ExecutedFunction = std::function<void(TimeDuration, TimeDuration, Entity*)>;

    class FunctionAction : public IAction
    {
        public:
            FunctionAction(ExecutedFunction passedFunction, Entity* entity)
				: storedFunction(passedFunction), EntityObj(entity) {};

			TimeDuration GetTotalTime() const override { return TimeDuration(5); }

			void OnUpdate(TimeDuration deltaTime) override
            {
				TotalDuration += deltaTime;
                storedFunction(TotalDuration, deltaTime, EntityObj);
            }

        private:
			TimeDuration TotalDuration = TimeDuration(0);
            ExecutedFunction storedFunction;
			Entity* EntityObj;
    }; 
} //namespace Poly