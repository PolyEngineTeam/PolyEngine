#pragma once

#include <Defines.hpp>
#include <ECS/ComponentIterator.hpp>

namespace Poly
{
    class Scene;
    class IAllocatorIterator;

    class ENGINE_DLLEXPORT SceneComponentIteratorImpl : public IComponentIteratorImpl
    {
        public:
            static SceneComponentIteratorImpl ConstructBegin(Scene* scene, size_t primaryComponentID);
            static SceneComponentIteratorImpl ConstructEnd(Scene* scene, size_t primaryComponentID);
            bool operator==(const IComponentIteratorImpl& rhs) const override;
            bool operator!=(const IComponentIteratorImpl& rhs) const override;
            ComponentBase* Get() const override;
            void Increment() override;
            bool IsValid() const override;
        private:
            SceneComponentIteratorImpl(std::unique_ptr<IAllocatorIterator> begin, std::unique_ptr<IAllocatorIterator> end);
            std::unique_ptr<IAllocatorIterator> Iterator;
            std::unique_ptr<IAllocatorIterator> End;
    };
}