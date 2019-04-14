#pragma once

#include <Defines.hpp>
#include <ECS/ComponentIterator.hpp>

namespace Poly
{
    class Entity;
    class IAllocatorIterator;

    class ENGINE_DLLEXPORT SceneComponentIteratorPolicy : public IEntityIteratorPolicy
    {
        public:
            SceneComponentIteratorPolicy(Entity* entity, size_t primaryComponentID);
            bool operator==(const IEntityIteratorPolicy& rhs) const override;
            bool operator!=(const IEntityIteratorPolicy& rhs) const override;
            Entity* Get() const override;
            void Increment() override;
            bool IsValid() const override;
        private:
            Entity* Match;
            std::unique_ptr<IAllocatorIterator> Iterator;
            std::unique_ptr<IAllocatorIterator> End;
    };

}