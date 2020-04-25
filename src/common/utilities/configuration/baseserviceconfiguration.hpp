#ifndef BASESERVICECONFIGURATION_HPP
#define BASESERVICECONFIGURATION_HPP

#include "compat.hpp"
#include <QObject>
#include <QTimer>
#include <QtDebug>

#include <QCoreApplication>

#include "servicelocator.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/initialize_trait.hpp"

#include "idtypes/persistentid.hpp"

#define ASSERT_IMPLEMENTATION_HINTS(Interface, Impl) \
static_assert( !implemented_as_QObject<Interface>::value || std::is_base_of<QObject, Impl>::value, "This implementation was expected to be QObject, but was not." );

/**
 * @class BaseServiceConfiguration
 * @brief
 * Used to configure the ServiceLocator
 * 
 * Inherit from this class to define a configuration for the ServiceLocator.
 * An instance of a subclass of BaseServiceConfiguration can be used to
 * initialize and destroy the ServiceLocator instance.
 */
class ADDLE_COMMON_EXPORT BaseServiceConfiguration
{
public:

    /**
     * Create the ServiceLocator and configure it.
     */
    void initialize()
    {
#ifdef ADDLE_DEBUG
        qDebug() << qUtf8Printable(QCoreApplication::translate(
            "BaseServiceConfiguration",
            "Initializing service locator."
        ));
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
        qDebug() << qUtf8Printable(QCoreApplication::translate(
            "BaseServiceConfiguration",
            "Destroying service locator."
        ));
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
};

#endif // BASESERVICECONFIGURATION_HPP