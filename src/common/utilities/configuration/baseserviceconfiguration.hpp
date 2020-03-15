#ifndef BASESERVICECONFIGURATION_HPP
#define BASESERVICECONFIGURATION_HPP

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
class BaseServiceConfiguration
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
    
    /**
     * Call this function from inside configure() to assign a concrete
     * `Factory` to each `Interface`.
     * 
     * @tparam Interface
     * The interface that this factory will be used with
     * 
     * @tparam Factory
     * The factory class to be used to create objects that implement
     * `Interface`
     * 
     * @note
     * `Factory` must inherit IFactory, and must expose a constructor
     * requiring no arguments.
     */
    template<class Interface, class Impl, class Factory, typename... ArgsType>
    void registerFactory(ArgsType... args)
    {
        Factory* factory = new Factory(args...);
        registerFactory<Interface, Impl, Factory>(factory);
    }

    template<class Interface, class Impl, class Factory>
    void registerFactory(Factory* factory)
    {
        static_assert(
            std::is_base_of<IFactory, Factory>::value,
            "Factory must inherit IFactory"
        );
        static_assert(
            std::is_base_of<Interface, Impl>::value,
            "Impl must inherit Interface"
        );
        static_assert(
            is_makeable<Interface>::value,
            "Interface must be makeable"
        );

        ASSERT_IMPLEMENTATION_HINTS(Interface, Impl);

        std::type_index interfaceIndex(typeid(Interface));

        factory->setServiceLocator(ServiceLocator::_instance);
        ServiceLocator::_instance->_factoryRegistry[interfaceIndex] = factory;
    }
};

#endif // BASESERVICECONFIGURATION_HPP