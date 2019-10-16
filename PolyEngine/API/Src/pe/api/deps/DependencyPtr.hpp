#pragma once

#include <pe/Defines.hpp>

#include <pe/api/deps/IDependencyPtr.hpp>
#include <pe/api/deps/DependencyManager.hpp>

namespace pe::api::deps {

template <typename T>
class DependencyPtr final : public IDependencyPtr
{   
public:
    DependencyPtr() { DependencyManager::get().registerDependencyPtr(this); }
    ~DependencyPtr() { DependencyManager::get().unregisterDependencyPtr(this); }

    virtual IDependency* getMutable() const
    { 
        STATIC_ASSERTE(!std::is_const<T>::value, "Cannot cast const to mutable.");
        return m_ptr;
    }
    virtual const IDependency* get() const { return m_ptr; }

    T& operator*() const noexcept { return m_ptr; }
    T* operator->() const noexcept { return m_ptr; }

    explicit operator bool() const noexcept { return m_ptr != nullptr; }

protected:
    virtual void set(IDependency* dep) { m_ptr = dep; }

private:
    // Not copyable object, must be managed by DependencyManager
    DependencyPtr(const DependencyPtr&) = delete;
    DependencyPtr(DependencyPtr&&) = delete;
    DependencyPtr& operator=(const DependencyPtr&) = delete;
    DependencyPtr& operator=(DependencyPtr&&) = delete;

    T* m_ptr = nullptr;
};

}