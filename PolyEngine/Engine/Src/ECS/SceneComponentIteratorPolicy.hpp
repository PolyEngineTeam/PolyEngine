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
            static SceneComponentIteratorPolicy ConstructBegin(Entity* entity, size_t primaryComponentID);
            static SceneComponentIteratorPolicy ConstructEnd(Entity* entity, size_t primaryComponentID);
            bool operator==(const IEntityIteratorPolicy& rhs) const override;
            bool operator!=(const IEntityIteratorPolicy& rhs) const override;
            Entity* Get() const override;
            void Increment() override;
            bool IsValid() const override;
        private:
            SceneComponentIteratorPolicy(Entity* entity, std::unique_ptr<IAllocatorIterator> begin, std::unique_ptr<IAllocatorIterator> end);
            Entity* Match;
            std::unique_ptr<IAllocatorIterator> Iterator;
            std::unique_ptr<IAllocatorIterator> End;
    };

}