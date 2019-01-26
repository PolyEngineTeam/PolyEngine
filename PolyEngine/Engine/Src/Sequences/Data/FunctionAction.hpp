#pragma once

#include <Defines.hpp>
#include <IAction.hpp>

namespace Poly
{
    class Scene;
    using ExecutedFunction = std::function<void(TimeDuration, RTTIBase*)>;

    class FunctionAction : public IAction
    {
        public:
            FunctionAction(ExecutedFunction passedFunction, RTTIBase* passedBase) : storedFunction(passedFunction), storedBase(passedBase) {};
            void OnUpdate(TimeDuration deltaTime)
            {
                storedFunction(deltaTime, storedBase);
            }
        private:
            ExecutedFunction storedFunction;
            RTTIBase* storedBase;
    }; 
} //namespace Poly