#include "utils.hpp"
#include "servicelocator.hpp"
#include "utilities/configuration/servicelocatorcontrol.hpp"

using namespace Addle;

ServiceLocator* ServiceLocator::_instance = nullptr;

ServiceLocator::~ServiceLocator()
{
    for (auto i = _objects.keyValueBegin(); i != _objects.keyValueEnd(); ++i)
    {
        auto index = (*i).first;
        auto service = (*i).second;
        auto factory = getFactory(FactorySelector(index.first, index.second));
        
        ADDLE_ASSERT(factory);
        
        factory->delete_(service);
    }
    for (auto config : noDetach(_configs))
    {
        delete config;
    }
}

IFactory* ServiceLocator::getFactory(const FactorySelector& selector)
{
    auto indiscIndex = qMakePair(selector.interfaceType(), selector.id());
    if (_indiscriminateFactories.contains(indiscIndex))
    {
        return _indiscriminateFactories.value(indiscIndex);
    }
    else
    {    
        bool isIndisc;
        for (auto config : noDetach(_configs))
        {
            auto factory = config->factory(selector, &isIndisc);
            if (factory)
            {
                if (isIndisc)
                    _indiscriminateFactories.insert(indiscIndex, factory);
                return factory;
            }
        }
    }
    return nullptr;
}

void ServiceLocator::addConfig(IServiceConfig* config)
{
    _configs.append(config);
    
    {
        auto persistentObjects = config->persistentObjects();
        auto&& begin = persistentObjects.constKeyValueBegin();
        auto&& end = persistentObjects.constKeyValueEnd();
        for (auto i = begin; i != end; ++i)
        {
            auto typeIndex = (*i).first;
            auto ids = (*i).second;
            for (auto id : noDetach(ids))
            {
                _knownIds[typeIndex].insert(id);
                _pendingPersistentObjects.insert(id, config);
            }
        }
    }
    
    // build cache
    // detect redundant or unreachable factories?
}


// {
//         todo: thread safety
//         
//         std::type_index interfaceIndex(typeid(Interface));
//         
//         if (_serviceInitMutexes.contains(interfaceIndex))
//         {
//             An initialization mutex exists for this service, i.e., it is
//             currently being constructed in another thread. We will wait
//             until it is finished.
// 
//             auto mutex = _serviceInitMutexes.value(interfaceIndex);
//             mutex->lock();
//             mutex->unlock();
//         }
// 
//         if (_services.contains(interfaceIndex))
//         {   
//             This service has already been made. Return the locator's instance.
// 
//             void* service = _services[interfaceIndex];
//             return reinterpret_cast<Interface*>(service);
//         }
//         else
//         {
//             auto mutex = QSharedPointer<QMutex>(new QMutex);
//             _serviceInitMutexes.insert(interfaceIndex, mutex);
//             mutex->lock();
// 
//             if (!_factoriesByType.contains(interfaceIndex))
//             {         
// #ifdef ADDLE_DEBUG
//                 ServiceNotFoundException ex(typeid(Interface).name());
// #else
//                 ServiceNotFoundException ex;
// #endif 
//                 ADDLE_THROW(ex);
//             }
// 
//             static_assert(
//                 !needs_init<Interface>(0),
//                 "Service types must not expect initialization"
//             );
// 
//             Interface* service = make_p<Interface>();
//             service->setServiceLocator(this);
//             _services[interfaceIndex] = reinterpret_cast<void*>(service);
// 
//             _serviceInitMutexes.remove(interfaceIndex);
//             mutex->unlock();
// 
//             return service;
//         }
//     }
//     
//        
//     internal get by interface and id
//     template<class Interface, class IdType>
//     Interface* get_p(IdType id)
//     {
//         static_assert(
//             Traits::is_gettable_by_id<Interface>::value,
//             "Interface must be gettable by id."
//         );
//         static_assert(
//             std::is_base_of<AddleId, IdType>::value,
//             "IdType must be AddleId"
//         );
// 
//         mutex
//     
//         auto index = qMakePair(id, std::type_index(typeid(Interface)));
// 
//         if (_persistentObjectsById.contains(index))
//         {
//             return reinterpret_cast<Interface*>(_persistentObjectsById[index]);
//         }
//         else 
//         {
//             if (!_factoriesById.contains(index))
//             {
// #ifdef ADDLE_DEBUG
//                 PersistentObjectNotFoundException ex(typeid(Interface).name(), id);
// #else
//                 PersistentObjectNotFoundException ex;
// #endif 
//                 ADDLE_THROW(ex);
//             }
// 
//             static_assert(
//                 !needs_init<Interface>(0),
//                 "Persistent object types must not expect initialization"
//             );
// 
//             Interface* object = make_p<Interface>(id);
//             _persistentObjectsById[index] = object;
// 
//             return object;
//         }
//     }
//     
//         {
//         static_assert(
//             Traits::is_makeable<Interface>::value,
//             "Interface must be makeable"
//         );
// 
// 
//         auto index = std::type_index(typeid(Interface));
// 
//         if (!_factoriesByType.contains(index))
//         {

//         }
// 
//         const IFactory* factory = _factoriesByType[index];
//         Interface* product = reinterpret_cast<Interface*>(factory->make());
// 
//         return product;
//     }
// 
//     internal make object by interface and id
//     template<class Interface, class IdType>
//     Interface* make_p(IdType id)
//     {
//         static_assert(
//             Traits::is_makeable_by_id<Interface>::value,
//             "Interface must be makeable by id"
//         );
//         static_assert(
//             std::is_same<typename Traits::id_type<Interface>::type, IdType>::value,
//             "Interface must be makeable by id of type IdType"
//         );
//         static_assert(
//             std::is_base_of<AddleId, IdType>::value,
//             "IdType must be AddleId"
//         );
// 
//         auto index = qMakePair(id, std::type_index(typeid(Interface)));
// 
//         if (!_factoriesById.contains(index))
//         {
// #ifdef ADDLE_DEBUG
//             FactoryNotFoundException ex(typeid(Interface).name(), id);
// #else
//             FactoryNotFoundException ex;
// #endif
//             ADDLE_THROW(ex);
//         }
// 
//         const IFactory* factory = _factoriesById[index];
//         Interface* product = reinterpret_cast<Interface*>(factory->make());
// 
//         return product;
//     }
