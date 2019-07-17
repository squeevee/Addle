/**
 * Addle source code
 * Copyright 2019 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#ifndef SERVICELOCATOR_H
#define SERVICELOCATOR_H

#include <exception>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>

#include "interfaces/services/ifactory.h"
#include "interfaces/services/ifactoryproduct.h"
#include "interfaces/services/iservice.h"
#include "interfaces/services/iservicelocator.h"

/**
 * Use the public static members of this class to access services and to use
 * object factories.
 * 
 * The ServiceLocator serves as a mediator between other classes in the
 * application, providing them access to each other without entangling them in
 * a dependency web. The Service Locator is itself essentially only an interface
 * relying on an external configuration to define its full behavior.
 * 
 * Note 1: Accessing ServiceLocator uses runtime type checking and
 * std::unordered_map accessors. Be careful of situations where this may be too
 * expensive.
 * 
 * Note 2: Exceptions thrown by ServiceLocator and its accessories are of type
 * std::exception, which are not thread-safe in Qt.
 */
class ServiceLocator : public IServiceLocator
{
public:
    /**
     * Provides a reference to the ServiceLocator's instance of the service
     * specified by Interface. If no such instance has yet been created, then
     * a new one is created.
     * 
     * Interface must inherit IService.
     */
    template<class Interface>
    static Interface& get()
    {
        static_assert(
            std::is_base_of<IService, Interface>::value,
            "Interface must inherit IService"
        );

        if (!_instance)
            throw ServiceLocator::ServiceLocatorNotInitializedException();

        return *_instance->get_p<Interface>();
    }

    /**
     * Creates a new object that implements the interface specified by
     * Interface, and returns a pointer to this new object. The caller is
     * responsible for the memory management of this object.
     */
    template<class Interface>
    static Interface* make()
    {
        if (!_instance)
            throw ServiceLocator::ServiceLocatorNotInitializedException();

        return _instance->make_p<Interface>();
    }

    /**
     * Special overload of ServiceLocator::make<Interface>() for convenience.
     * Creates a new object that implements the interface specified by
     * Interface, then calls the initialize function on that object with the
     * arguments given. The caller is responsible for the memory management
     * of this object.
     * 
     * The arguments of this function must be the same as if calling
     * Interface::initialize()
     */
    template<class Interface, typename... ArgTypes>
    static Interface* make(ArgTypes ...args)
    {
        if (!_instance)
            throw ServiceLocator::ServiceLocatorNotInitializedException();
            
        Interface* result = _instance->make_p<Interface>();
        result->initialize(args...);

        return result;
    }
    
    /**
     * Inherit from this class to define a configuration for the ServiceLocator.
     * An instance of a subclass of AbstractConfiguration can be used to
     * initialize and destroy the ServiceLocator instance.
     */
    class AbstractConfiguration
    {
    public:
        /**
         * Create the ServiceLocator and configure it.
         */
        void initialize();
        
        /**
         * Destroy the ServiceLocator.
         */
        void destroy();
        
    protected:
        /**
         * Implement this function and call registerFactory() to define the
         * ServiceLocator's behavior.
         */
        virtual void configure() = 0;
        
        /**
         * Call this function from inside configure() to assign a concrete
         * Factory to each Interface.
         * 
         * Factory must inherit IFactory, and must expose a constructor
         * requiring no arguments.
         */
        template<class Interface, class Factory>
        void registerFactory()
        {
            static_assert(
                std::is_base_of<IFactory, Factory>::value,
                "Factory must inherit IFactory"
            );

            std::type_index interfaceIndex(typeid(Interface));
            Factory* factory = new Factory();
            factory->setServiceLocator(ServiceLocator::_instance);
            ServiceLocator::_instance->_factoryregistry[interfaceIndex] = factory;
        }
    };

    /**
     * Thrown if the ServiceLocator is expectd to be initialized, but isn't.
     */
    class ServiceLocatorNotInitializedException : public std::logic_error
    {
    public: ServiceLocatorNotInitializedException();
    };

    /**
     * Thrown if the ServiceLocator is not expected to be initialized, but is.
     */
    class ServiceLocatorAlreadyInitializedException : public std::logic_error
    {
    public: ServiceLocatorAlreadyInitializedException();
    };
    
    /**
     * Thrown if no factory is found for a given interface
     */
    class FactoryNotFoundException : public std::logic_error
    {
    public:
        FactoryNotFoundException(
                const char* requestedInterfaceName,
                int factoryCount
            );

        // The name of the interface type according to <typeinfo>
        const char* getRequestedInterfaceName() const
            { return _requestedInterfaceName; }
        // The number of factories that have been registered in ServiceLocator.
        int getFactoryCount() const { return _factoryCount; }
        
    private:
        const char* _requestedInterfaceName;
        int _factoryCount;
    };
    
    /**
     * Thrown if no service was found for a given interface, and none could
     * be made.
     */
    class ServiceNotFoundException : public std::logic_error
    {
    public:
        ServiceNotFoundException(
                const char* requestedInterfaceName,
                int serviceCount,
                int factoryCount
            );
        
        // The name of the interface type according to <typeinfo>
        const char* getRequestedInterfaceName() const
            { return _requestedInterfaceName; }
        // The number of services owned by ServiceLocator.
        int getServiceCount() const { return _serviceCount; }
        // The number of factories that have been registered in ServiceLocator.
        int getFactoryCount() const { return _factoryCount; }
        
    private:
        const char* _requestedInterfaceName;
        int _serviceCount;
        int _factoryCount;
    };
    
    /**
     * Thrown if the product from a factory did not implement the factory's 
     * interface.
     */
    class InvalidFactoryProductException : public std::logic_error
    {
    public:
        InvalidFactoryProductException(
                const char* requestedInterfaceName,
                const char* factoryProductName,
                const char* factoryName
            );
                
        // The name of the interface type according to <typeinfo>
        const char* getRequestedInterfaceName() { return _requestedInterfaceName; }
        // The name of the FactoryProduct according to itself
        const char* getFactoryProductName() { return _factoryProductName; }
        // The name of the Factory according to itself
        const char* getFactoryName() { return _factoryName; }
        
    private:
        const char* _requestedInterfaceName;
        const char* _factoryProductName;
        const char* _factoryName;
    };

private:

    ServiceLocator() {}
    virtual ~ServiceLocator();

    static ServiceLocator* _instance;
    
    std::unordered_map<std::type_index, IFactory*> _factoryregistry;
    std::unordered_map<std::type_index, IService*> _serviceregistry;
    
    // Private internals of getting a service
    template<class Interface>
    Interface* get_p()
    {
        std::type_index interfaceIndex(typeid(Interface));
        
        if (_serviceregistry.find(interfaceIndex) == _serviceregistry.end())
        {
            if (_factoryregistry.find(interfaceIndex) == _factoryregistry.end())
            {
                throw ServiceNotFoundException(
                        typeid(Interface).name(),
                        _serviceregistry.size(),
                        _factoryregistry.size()
                    );
            }
            
            Interface* service = make_p<Interface>();
            service->setServiceLocator(this);
            _serviceregistry[interfaceIndex] = service;
            return service;
        }
        else
        {
            Interface* service = dynamic_cast<Interface*>(
                _serviceregistry.at(interfaceIndex)
            );
        }
    }

    // Private internals of making an object
    template<class Interface>
    Interface* make_p()
    {
        std::type_index interfaceIndex(typeid(Interface));
        
        if (_factoryregistry.find(interfaceIndex) == _factoryregistry.end())
        {
            throw FactoryNotFoundException(
                    typeid(Interface).name(),
                    _factoryregistry.size()
                );
        }
        
        IFactory* factory = _factoryregistry.at(interfaceIndex);
        IFactoryProduct* product = factory->make();
        Interface* result = dynamic_cast<Interface*>(product);

        if (!result)
        {
            throw InvalidFactoryProductException(
                    typeid(Interface).name(),
                    product->getFactoryProductName(),
                    factory->getFactoryName()
                );
        }
    }
    
    friend class AbstractConfigure;
};

#endif //SERVICELOCATOR_H
