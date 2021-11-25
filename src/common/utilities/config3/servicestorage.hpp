#pragma once 

#include <QAtomicPointer>
#include <QMutex>

#include <memory> // std::shared_ptr
#include <tuple>

#include <boost/mp11.hpp>
#include "./aux.hpp" // di_service_storage_base

namespace Addle::aux_config3 {
    
template<typename Interface, typename Impl>
struct _storage_pair { using interface_t = Interface; using impl_t = Impl; };

class di_service_storage_base {};

template<typename... TServiceMap>
class ServiceStorage : public di_service_storage_base
{
    static_assert(boost::mp11::mp_is_map<ServiceStorage>::value);
    
    template<typename TService>
    struct Entry
    {
        ~Entry()
        {
#ifdef ADDLE_DEBUG
            assert(initMutex.tryLock());
            initMutex.unlock();
#endif
            TService* service = ptr.loadRelaxed();
            if (service) delete service;
        }
        
        QAtomicPointer<TService> ptr = nullptr;
        mutable QMutex initMutex;
    };
    
    using data_t = boost::mp11::mp_apply<
            std::tuple,
            boost::mp11::mp_transform<
                Entry, boost::mp11::mp_transform<boost::mp11::mp_second, ServiceStorage>
            >
        >;
        
    // e.g.:
    // TServiceMap = { _storage_pair<ISpiff, Spiff>, _storage_pair<IFreem, Freem> };
    // data_t = std::tuple<
    //         std::pair<QAtomicPointer<Spiff>, QMutex>,
    //         std::pair<QAtomicPointer<Freem>, QMutex>
    //     >;
    
public:
    ServiceStorage() : _data(std::make_shared<data_t>()) {}
    
    ServiceStorage(const ServiceStorage&) = default;
    ServiceStorage(ServiceStorage&&) = default;
    
    template<class Interface>
    using contains = boost::mp11::mp_map_contains<ServiceStorage, Interface>;
    
    template<class Interface>
    using service_impl_t = boost::mp11::mp_second<boost::mp11::mp_map_find<ServiceStorage, Interface>>;
    
    // Acquires the pointer to the service given by Interface, or nullptr if
    // the service has not been initialized. After a service has been 
    // initialized, it will remain so for the lifetime of this storage object. 
    // The returned type is a pointer to the implementation class.
    //
    // This function will not lock, provided that QAtomicPointer does not lock.
    template<class Interface>
    std::enable_if_t<contains<Interface>::value, service_impl_t<Interface>*> 
    getService() const noexcept
    {
        assert(_data);
        constexpr std::size_t I = boost::mp11::mp_find<
                boost::mp11::mp_map_keys<ServiceStorage>, Interface
            >::value;
            
        return static_cast<
                boost::mp11::mp_second<boost::mp11::mp_map_find<ServiceStorage, Interface>>*
            >(std::get<I>(*_data).ptr.loadRelaxed());
    }
    
    template<class Interface>
    std::enable_if_t<contains<Interface>::value>
    setService(Interface* service) noexcept
    {
        assert(_data);
#ifdef ADDLE_DEBUG
        assert(dynamic_cast< service_impl_t<Interface>* >(service));
#endif
        constexpr std::size_t I = boost::mp11::mp_find<
                boost::mp11::mp_map_keys<ServiceStorage>, Interface
            >::value;
            
        auto&& entry = std::get<I>(*_data);
        
        entry.initMutex.lock();
        auto former = entry.ptr.fetchAndStoreRelaxed(static_cast< service_impl_t<Interface>* >(service));
        entry.initMutex.unlock();
        
        if (Q_UNLIKELY(former)) delete former;
    }
    
    // Acquires the initialization lock for the service given by Interface and,
    // if it has not already been initialized, initializes the pointer with the
    // value returned by a call to `f()` and returns true. If the service has
    // already been initialized (e.g., by another thread while the lock was
    // being acquired), does nothing and returns false.
    template<class Interface, typename F>
    std::enable_if_t< contains<Interface>::value, std::pair<service_impl_t<Interface>*, bool> >
    initializeService(F&& f)
    {
        assert(_data);
        constexpr std::size_t I = boost::mp11::mp_find<
                boost::mp11::mp_map_keys<ServiceStorage>, Interface
            >::value;
        
        service_impl_t<Interface>* result;
        auto&& entry = std::get<I>(*_data);
        
        QMutexLocker lock(&entry.initMutex);
        
        if (Q_UNLIKELY( result = entry.ptr.loadRelaxed() )) return { result, false };
        assert(! entry.ptr.fetchAndStoreRelaxed((result = std::invoke(f))) );
        
        return { result, true };
    }
    
private:    
    std::shared_ptr<data_t> _data;
};

template<>
class ServiceStorage<> : public di_service_storage_base
{
public:
    template<class> using contains = std::false_type;
};

template<typename TDep>
using _make_storage_pair = _storage_pair<typename TDep::expected, typename TDep::given>;
    
template<typename T, typename U>
using _storage_pair_match_interfaces = std::is_same<typename T::interface_t, typename U::interface_t>;

template<typename... Interfaces>
auto make_service_storage()
{
    return ServiceStorage<boost::mp11::mp_list<Interfaces, Interfaces>...> {};
}

}
