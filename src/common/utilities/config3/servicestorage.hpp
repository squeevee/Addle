#pragma once 

#include <QAtomicPointer>
#include <memory> // std::shared_ptr
#include <tuple>

#include <boost/mp11.hpp>

namespace Addle::aux_config3 {
    
template<typename... TServiceMap>
class ServiceStorage
{
    using map_t = boost::mp11::mp_list<TServiceMap...>;
    static_assert(boost::mp11::mp_is_map<map_t>::value);
    
    using data_t = boost::mp11::mp_apply<std::tuple,
        boost::mp11::mp_transform_q<
            boost::mp11::mp_bind<std::pair, 
                boost::mp11::mp_bind<QAtomicPointer, boost::mp11::_1>, 
                QMutex
            >,
            boost::mp11::mp_transform<boost::mp11::mp_second, map_t>
        >>;        
    // e.g.:
    // TServiceMap = { mp_list<ISpiff, Spiff>, mp_list<IFreem, Freem> };
    // data_t = std::tuple<
    //         std::pair<QAtomicPointer<Spiff>, QMutex>,
    //         std::pair<QAtomicPointer<Freem>, QMutex>
    //     >;
    
public:
    ServiceStorage() : _data(std::make_shared<data_t>()) {}
    
    ServiceStorage(const ServiceStorage&) = default;
    ServiceStorage(ServiceStorage&&) = default;
    
    ~ServiceStorage() noexcept
    {
        boost::mp11::tuple_for_each(*_data,
            [&] (auto&& entry) {
    #ifdef ADDLE_DEBUG
                assert(entry.second.tryLock());
                entry.second.unlock();
    #endif
                auto service = entry.first.loadRelaxed();
                if (service) delete service;
            });
    }
    
    template<class Interface>
    using contains = boost::mp11::mp_map_contains<map_t, Interface>;
    
    // Acquires the pointer to the service given by Interface, or nullptr if
    // the service has not been initialized. After a service has been 
    // initialized, it will remain so for the lifetime of this object. The 
    // returned type is a pointer to the implementation class.
    //
    // This function's lockfree guarantee is only as strong as QAtomicPointer's
    // lockfree guarantee
    template<class Interface>
    std::enable_if_t<
        boost::mp11::mp_map_contains<map_t, Interface>::value,
        boost::mp11::mp_second<boost::mp11::mp_map_find<map_t, Interface>>*> 
    getService() const noexcept
    {
        assert(_data);
        constexpr std::size_t I = boost::mp11::mp_find<
                boost::mp11::mp_map_keys<map_t>, Interface
            >::value;
            
        return static_cast<
                boost::mp11::mp_second<boost::mp11::mp_map_find<map_t, Interface>>*
            >(std::get<I>(*_data).first.loadRelaxed());
    }
    
    // Acquires the initialization lock for the service given by Interface and,
    // if it has not already been initialized, initializes the pointer with the
    // value returned by a call to `f()` and returns true. If the service has
    // already been initialized (e.g., by another thread while the lock was
    // being acquired), does nothing and returns false.
    template<class Interface, typename F>
    std::enable_if_t<
        boost::mp11::mp_map_contains<map_t, Interface>::value,
        std::pair<boost::mp11::mp_second<boost::mp11::mp_map_find<map_t, Interface>>*, bool>>
    initializeService(F&& f)
    {
        assert(_data);
        constexpr std::size_t I = boost::mp11::mp_find<
                boost::mp11::mp_map_keys<map_t>, Interface
            >::value;
        
        auto&& entry = std::get<I>(*_data);
        QMutexLocker lock(&entry.second);
        
        boost::mp11::mp_second<boost::mp11::mp_map_find<map_t, Interface>>*
        result;
        
        if (Q_UNLIKELY((result = getService<Interface>()))) return { result, false };
        
        entry.first.storeRelaxed((result = std::invoke(f)));
        return { result, true };
    }
    
private:    
    std::shared_ptr<data_t> _data;
};

template<>
class ServiceStorage<>
{
public:
    ServiceStorage() = default;
    ServiceStorage(const ServiceStorage&) = default;
    ServiceStorage(ServiceStorage&&) = default;
    ~ServiceStorage() noexcept = default;
    
    template<class> using contains = std::false_type;
};

}
