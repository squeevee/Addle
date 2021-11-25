#pragma once

#include <QReadWriteLock>
#include <QMutex>

#include <typeindex>
#include <unordered_map>

namespace Addle::aux_config3 {

class DynamicServiceStorage
{
public:
    template<typename T>
    inline T* getService() const;
    
    template<typename T>
    T* extractService();
    
    template<typename T, typename F>
    std::pair<T*, bool> initializeService(F&& f);
    
private:
    struct Entry 
    {
        Entry(void (*deleter_)(void*), QAtomicPointer<void> ptr_ = nullptr)
            : deleter(deleter_), ptr(ptr_)
        {
            assert(deleter);
        }
        
        ~Entry() noexcept
        {
            assert(deleter);
            
#ifdef ADDLE_DEBUG
            assert(initMutex.tryLock());
            initMutex.unlock();
#endif      
            void* service = ptr.loadRelaxed();
            if (service) deleter(service);
        }
        
        void (*deleter)(void*);
        QAtomicPointer<void> ptr;
        QMutex initMutex;
    };
    
    template<typename Interface>
    static void _deleter_impl(void* obj) { delete static_cast<Interface*>(obj); }
    
    mutable std::unordered_map<std::type_index, Entry> _data;
    mutable QReadWriteLock _lock;
};

// Accesses the service object of type T. If no such service is managed by this
// storage, returns nullptr.
// Waits to acquire a read lock.
template<typename T>
inline T* DynamicServiceStorage::getService() const
{
    const QReadLocker lock(&_lock);
    
    auto find = qAsConst(_data).find(typeid(T));
    if (find == _data.cend()) 
        return nullptr;
    
    return (find != _data.cend()) ? reinterpret_cast<T*>((*find).second.ptr.loadRelaxed()) : nullptr;
}

// Takes ownership of the service object of type T, releasing it from storage.
// If no such service of type is managed by this storage, returns nullptr.
// Waits to acquire a write lock, and may wait longer if the object is being
// initialized.
template<typename T>
T* DynamicServiceStorage::extractService()
{
    const QWriteLocker lock1(&_lock);
    
    auto find = _data.find(typeid(T));
    if (find == _data.end())
        return nullptr;
        
    T* result;
    
    QMutexLocker lock2(&(*find).second.initMutex);
    
    // we assume that there would only be an entry with a null pointer because
    // it was added before we got the lock, but is not yet initialized.
    while(!( result = reinterpret_cast<T*>((*find).second.ptr.fetchAndStoreRelaxed(nullptr)) ))
    {
        lock2.unlock();
        lock2.relock();
    }
    lock2.unlock();
    
    _data.erase(find);
    
    return result;
}

// Gets a pointer to the service object of type T. If no such service object 
// already exists, this will invoke the given function object `f` to create it.
// Returns the service, and true if f was invoked, false if f was not invoked.
// Always acquires a write lock, if the service is not initialized it will 
// acquire a subsequent init lock.
template<typename T, typename F>
std::pair<T*, bool> DynamicServiceStorage::initializeService(F&& f)
{
    T* result;
    Entry* entry; 
    // only the destructor and extractService can invalidate the entry reference, 
    // and extractService will wait until the entry is initialized before doing 
    // so, we can safely keep this between locks
    
    {
        const QWriteLocker lock1(&_lock);
        
        auto it = _data.find(typeid(T));
        
        if (Q_LIKELY(it == _data.end()))
            std::tie(it, std::ignore) = _data.emplace(typeid(T), &_deleter_impl<T>);
        else if (Q_UNLIKELY( result = reinterpret_cast<T*>((*it).second.ptr.loadRelaxed()) ))
            return { result, false };
        
        entry = &((*it).second);
    }
    
    {
        QMutexLocker lock2(&entry->initMutex);
        
        if (Q_UNLIKELY( result = reinterpret_cast<T*>(entry->ptr.loadRelaxed()) ))
            return { result, false };
        
        assert(! entry->ptr.fetchAndStoreRelaxed( result = std::invoke(std::forward<F>(f)) ) );
        assert(result); 
        // null result at this point could cause extractService to loop forever
        // and doesn't make sense anyway.
        
        return { result, true };
    }
}
    
}
