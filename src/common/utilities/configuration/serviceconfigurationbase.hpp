/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef SERVICECONFIGURATIONBASE_HPP
#define SERVICECONFIGURATIONBASE_HPP

#include "compat.hpp"
#include <QObject>
#include <QTimer>
#include <QtDebug>

#include <QCoreApplication>

#include "servicelocator.hpp"
#include "interfaces/traits.hpp"



#include "idtypes/addleid.hpp"

// #define ASSERT_IMPLEMENTATION_HINTS(Interface, Impl) \
// static_assert( !implemented_as_QObject<Interface>::value || std::is_base_of<QObject, Impl>::value, "This implementation was expected to be QObject, but was not." );
namespace Addle {

/**
 * @class ServiceConfigurationBase
 * @brief
 * Used to configure the ServiceLocator
 * 
 * Inherit from this class to define a configuration for the ServiceLocator.
 * An instance of a subclass of ServiceConfigurationBase can be used to
 * initialize and destroy the ServiceLocator instance.
 */
class ADDLE_COMMON_EXPORT ServiceConfigurationBase
{
public:

    /**
     * Create the ServiceLocator and configure it.
     */
    void initialize()
    {
#ifdef ADDLE_DEBUG
        //% "Initializing service locator."
        qDebug() << qUtf8Printable(qtTrId("debug-messages.service-locator.initializing"));
#endif
        if (ServiceLocator::_instance)
            throw ServiceLocatorAlreadyInitializedException();

        ServiceLocator::_instance = new ServiceLocator();
        configure();
    }

    /**
     * Destroy the ServiceLocator.
     */
    void destroy()
    {
#ifdef ADDLE_DEBUG
        //% "Destroying service locator."
        qDebug() << qUtf8Printable(qtTrId("debug-messages.service-locator.destroying"));
#endif

        if (!ServiceLocator::_instance)
            throw ServiceLocatorNotInitializedException();

        delete ServiceLocator::_instance;
        ServiceLocator::_instance = nullptr;
    }

protected:
    /**
     * Implement this function and call registerFactory() to define the
     * ServiceLocator's behavior.
     */
    virtual void configure() = 0;
    
    template<class Interface, class IdType>
    void registerFactoryById(const IFactory* factory, IdType id)
    {
        // TODO: asserts
        
        auto index = qMakePair(id, std::type_index(typeid(Interface)));
        ServiceLocator::_instance->_factoriesById[index] = factory;
    }

    template<class Interface>
    void registerFactoryByType(const IFactory* factory)
    {
        // TODO: asserts
        
        std::type_index index(typeid(Interface));
        ServiceLocator::_instance->_factoriesByType[index] = factory;
    }

    template<class Interface, class IdType>
    void registerObjectById(Interface* object, IdType id)
    {
        ServiceLocator::_instance->_persistentObjectsById[
            qMakePair(id, std::type_index(typeid(Interface)))
        ] = object;
    }

    template<class Interface, class IdType, typename... ArgTypes>
    void buildPersistentObject(IdType id, ArgTypes... initArgs)
    {
        Interface* object = ServiceLocator::_instance->make_p<Interface>();
        object->initialize(initArgs...);
        registerObjectById(object, id);
    }
};
} // namespace Addle

#endif // SERVICECONFIGURATIONBASE_HPP