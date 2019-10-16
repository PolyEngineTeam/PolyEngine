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

    virtual DependencyBase* getMutable() const
    { 
        STATIC_ASSERTE(!std::is_const<T>::value, "Cannot cast const to mutable.");
        return m_ptr;
    }
    virtual const DependencyBase* get() const { return m_ptr; }

    T& operator*() const noexcept { return m_ptr; }
    T* operator->() const noexcept { return m_ptr; }

    explicit operator bool() const noexcept { return m_ptr != nullptr; }

    bool operator==(std::nullptr_t) const { return m_ptr == nullptr; }
    bool operator==(const T* rhs) const { return m_ptr == rhs; }
    bool operator==(const DependencyPtr<T>& rhs) const { return m_ptr == rhs.m_ptr; }
    
    friend std::ostream& operator<< (std::ostream& stream, const DependencyPtr<T>& ptr)
    { 
        return stream << "DependencyPtr[" << ptr.get() << "]";
    }
protected:
    virtual void inject(DependencyBase* dep)
    {
        // @todo(muniu) consider switching to rtti_cast
        // this is not a performance sensitive piece of code, so we may be ok with dynamic_cast
        ASSERTE(dep, "Cannot inject nullptr!");
        m_ptr = dynamic_cast<T*>(dep);
        ASSERTE(m_ptr, "Dependency injection failed while upcasting!");
    }

    virtual void reset() { m_ptr = nullptr; }

    virtual std::type_index getType() const { return typeid(T); }

private:
    // Not copyable object, must be managed by DependencyManager
    DependencyPtr(const DependencyPtr&) = delete;
    DependencyPtr(DependencyPtr&&) = delete;
    DependencyPtr& operator=(const DependencyPtr&) = delete;
    DependencyPtr& operator=(DependencyPtr&&) = delete;

    T* m_ptr = nullptr;
};

}