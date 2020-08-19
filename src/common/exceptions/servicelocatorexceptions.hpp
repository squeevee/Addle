/**
 * Addle source code
 * 
 * @file initializeexceptions.h
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef SERVICELOCATOREXCEPTIONS_HPP
#define SERVICELOCATOREXCEPTIONS_HPP

#include <QStringBuilder>
#include <stdexcept>

#include "addleexception.hpp"

namespace Addle {

DECL_LOGIC_ERROR(ServiceLocatorException)
class ADDLE_COMMON_EXPORT ServiceLocatorException : public AddleException
{
#ifdef ADDLE_DEBUG
public:
    ServiceLocatorException(const QString what)
        : AddleException(what)
    {
    }
#else
public:
    ServiceLocatorException() = default;
#endif
public:
    virtual ~ServiceLocatorException() = default;
};

DECL_LOGIC_ERROR(ServiceLocatorNotInitializedException)

/**
 * @class ServiceLocatorNotInitializedException
 * @brief Thrown if the ServiceLocator is expected to be initialized, but isn't,
 * (e.g., the ServiceLocator was not configured)
 */
class ADDLE_COMMON_EXPORT ServiceLocatorNotInitializedException: public ServiceLocatorException
{
    ADDLE_EXCEPTION_BOILERPLATE(ServiceLocatorNotInitializedException)
#ifdef ADDLE_DEBUG
public:
    ServiceLocatorNotInitializedException()
        : ServiceLocatorException(
            //% "The service locator is not initialized."
            qtTrId("debug-messages.service-locator-error.not-initialized")
        )
    {
    }
#else
    ServiceLocatorNotInitializedException() = default;
#endif

    virtual ~ServiceLocatorNotInitializedException() = default;
};

/**
 * @class ServiceLocatorAlreadyInitializedException
 * @brief Thrown if the ServiceLocator is not expected to be initialized, but
 * is, (e.g., there were multiple attempts to configure the ServiceLocator)
 */
class ADDLE_COMMON_EXPORT ServiceLocatorAlreadyInitializedException : public ServiceLocatorException
{
    ADDLE_EXCEPTION_BOILERPLATE(ServiceLocatorAlreadyInitializedException)

public: 
#ifdef ADDLE_DEBUG
    ServiceLocatorAlreadyInitializedException()
        : ServiceLocatorException(
            //% "The service locator has already been initialized."
            qtTrId("debug-messages.service-locator-error.already-initialized")
        )
    {
    }
#else
public:
    ServiceLocatorAlreadyInitializedException() = default;
#endif
public:
    virtual ~ServiceLocatorAlreadyInitializedException() = default;
};

/**
 * @class FactoryNotFoundException
 * @brief Thrown by ServiceLocator if no factory is found for a given interface
 */
class ADDLE_COMMON_EXPORT FactoryNotFoundException : public ServiceLocatorException
{
    ADDLE_EXCEPTION_BOILERPLATE(FactoryNotFoundException)
#ifdef ADDLE_DEBUG
public:
    FactoryNotFoundException(
            const char* requestedInterfaceName,
            int factoryCount
        )
        : ServiceLocatorException(
            //% "No factory was found for the requested interface.\n"
            //% "requestedInterfaceName: \"%1\"\n"
            //% "          factoryCount: %2"
            qtTrId("debug-messages.service-locator-error.factory-not-found")
                .arg(requestedInterfaceName)
                .arg(factoryCount)
        ),
        _requestedInterfaceName(requestedInterfaceName),
        _factoryCount(factoryCount)
    {
    }
private:
    const char* _requestedInterfaceName;
    int _factoryCount;
#else
public:
    FactoryNotFoundException() = default;
#endif
public:
    virtual ~FactoryNotFoundException() = default;
};

/**
 * @class ServiceNotFoundException
 * @brief Thrown by ServiceLocator if no service was found for a given
 * interface, and one could not be made, (i.e., there also was no factory for
 * it).
 */
class ADDLE_COMMON_EXPORT ServiceNotFoundException : public ServiceLocatorException
{
    ADDLE_EXCEPTION_BOILERPLATE(ServiceNotFoundException)
#ifdef ADDLE_DEBUG
public:
    ServiceNotFoundException(
            const char* requestedInterfaceName,
            int serviceCount,
            int factoryCount
        ) : ServiceLocatorException(
            //% "No service was found for the requested interface.\n"
            //% "requestedInterfaceName: \"%1\"\n"
            //% "          serviceCount: %2\n"
            //% "          factoryCount: %3"
            qtTrId("debug-messages.service-locator-error.service-not-found")
                .arg(requestedInterfaceName)
                .arg(serviceCount)
                .arg(factoryCount)
        ),
        _requestedInterfaceName(requestedInterfaceName),
        _serviceCount(serviceCount),
        _factoryCount(factoryCount)
    {
    }    
private:
    const char* _requestedInterfaceName;
    int _serviceCount;
    int _factoryCount;
#else
public:
    ServiceNotFoundException() = default;
#endif
public:
    virtual ~ServiceNotFoundException() = default;
};

/**
 * @class InvalidFactoryProductException
 * @brief Thrown by ServiceLocator if the product from a factory did not 
 * implement the factory's interface.
 */
// OBSOLETE?
class ADDLE_COMMON_EXPORT InvalidFactoryProductException : public ServiceLocatorException
{
    ADDLE_EXCEPTION_BOILERPLATE(InvalidFactoryProductException)

#ifdef ADDLE_DEBUG
public:
    InvalidFactoryProductException(
            const char* requestedInterfaceName,
            const char* factoryProductName,
            const char* factoryName
        ) : ServiceLocatorException(
            //% "An invalid factory product was created.\n"
            //% "requestedInterfaceName: \"%1\"\n"
            //% "    factoryProductName: \"%2\"\n"
            //% "           factoryName: \"%3\""
            qtTrId("debug-messages.service-locator-error.invalid-product")
                .arg(requestedInterfaceName)
                .arg(factoryProductName)
                .arg(factoryName)),
        _requestedInterfaceName(requestedInterfaceName),
        _factoryProductName(factoryProductName),
        _factoryName(factoryName)
    {  
    }    
private:
    const char* _requestedInterfaceName;
    const char* _factoryProductName;
    const char* _factoryName;
#else
public:
    InvalidFactoryProductException() = default;
#endif
public:
    virtual ~InvalidFactoryProductException() = default;
};

class ADDLE_COMMON_EXPORT FactoryException : public ServiceLocatorException
{
    ADDLE_EXCEPTION_BOILERPLATE(FactoryException)

    enum Why
    {
        wrong_thread
    };

    #ifdef ADDLE_DEBUG
public:
    FactoryException(
            Why why,
            const char* factoryProductName,
            const char* factoryName
        ) : ServiceLocatorException(
            //% "An error occurred in a factory.\n"
            //% "    factoryProductName: \"%1\"\n"
            //% "           factoryName: \"%2\"\n"
            //% "              why code: %3"
            qtTrId("debug-messages.service-locator-error.factory-exception")
                .arg(factoryProductName)
                .arg(factoryName)
                .arg(why)
        ),
        _factoryProductName(factoryProductName),
        _factoryName(factoryName),
        _why(why)
    {  
    }    
private:
    const char* _factoryProductName;
    const char* _factoryName;
#else
public:
    FactoryException(Why why)
        : _why(why)
    {
    }
#endif
public:
    virtual ~FactoryException() = default;

private:
    const Why _why;
};

} // namespace Addle

#endif // SERVICELOCATOREXCEPTIONS_HPP