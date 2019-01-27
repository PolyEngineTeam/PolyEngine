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
            FunctionAction(TimeDuration duration, ExecutedFunction passedFunction, Entity* entity)
				: Duration(duration), storedFunction(passedFunction), EntityObj(entity) {};

			TimeDuration GetDuration() const override { return Duration; }

			void OnUpdate(TimeDuration deltaTime) override
            {
				TotalTime += deltaTime;
                storedFunction(TotalTime, deltaTime, EntityObj);
            }

        private:
			TimeDuration Duration = TimeDuration(0);
			TimeDuration TotalTime = TimeDuration(0);
            ExecutedFunction storedFunction;
			Entity* EntityObj;
    }; 
} //namespace Poly