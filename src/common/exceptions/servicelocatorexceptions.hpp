/**
 * Addle source code
 * 
 * @file initializeexceptions.h
 * @copyright Copyright 2019 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef SERVICELOCATOREXCEPTIONS_HPP
#define SERVICELOCATOREXCEPTIONS_HPP

#include <QStringBuilder>
#include <stdexcept>

#include "baseaddleexception.hpp"

#include "utilities/qt_extensions/translation.hpp"

DECL_LOGIC_ERROR(ServiceLocatorException)
class ServiceLocatorException : public BaseAddleException
{
#ifdef ADDLE_DEBUG
public:
    ServiceLocatorException(const QString what)
        : BaseAddleException(what)
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
class ServiceLocatorNotInitializedException: public ServiceLocatorException
{
    ADDLE_EXCEPTION_BOILERPLATE(ServiceLocatorNotInitializedException)
#ifdef ADDLE_DEBUG
public:
    ServiceLocatorNotInitializedException()
        : ServiceLocatorException(QCoreApplication::translate(
            "ServiceLocatorAlreadyInitializedException", 
            "The service locator is not initialized."
        ))
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
class ServiceLocatorAlreadyInitializedException : public ServiceLocatorException
{
    ADDLE_EXCEPTION_BOILERPLATE(ServiceLocatorAlreadyInitializedException)

public: 
#ifdef ADDLE_DEBUG
    ServiceLocatorAlreadyInitializedException()
        : ServiceLocatorException(QCoreApplication::translate(
            "ServiceLocatorAlreadyInitializedException", 
            "The service locator has already been initialized."
        ))
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
class FactoryNotFoundException : public ServiceLocatorException
{
    ADDLE_EXCEPTION_BOILERPLATE(FactoryNotFoundException)
#ifdef ADDLE_DEBUG
public:
    FactoryNotFoundException(
            const char* requestedInterfaceName,
            int factoryCount
        )
        : ServiceLocatorException(fallback_translate("FactoryNotFoundException", "what", QStringLiteral(
            "No factory was found for the requested interface.\n"
            "requestedInterfaceName: \"%1\"\n"
            "          factoryCount: %2"))
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
class ServiceNotFoundException : public ServiceLocatorException
{
    ADDLE_EXCEPTION_BOILERPLATE(ServiceNotFoundException)
#ifdef ADDLE_DEBUG
public:
    ServiceNotFoundException(
            const char* requestedInterfaceName,
            int serviceCount,
            int factoryCount
        ) : ServiceLocatorException(fallback_translate("ServiceNotFoundException", "what", QStringLiteral(
            "No service was found for the requested interface.\n"
            "requestedInterfaceName: \"%1\"\n"
            "          serviceCount: %2\n"
            "          factoryCount: %3"))
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
class InvalidFactoryProductException : public ServiceLocatorException
{
    ADDLE_EXCEPTION_BOILERPLATE(InvalidFactoryProductException)

#ifdef ADDLE_DEBUG
public:
    InvalidFactoryProductException(
            const char* requestedInterfaceName,
            const char* factoryProductName,
            const char* factoryName
        ) : ServiceLocatorException(fallback_translate("InvalidFactoryProductException", "what", QStringLiteral(
            "An invalid factory product was created.\n"
            "requestedInterfaceName: \"%1\"\n"
            "    factoryProductName: \"%2\"\n"
            "           factoryName: \"%3\""))
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

class FactoryException : public ServiceLocatorException
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
        ) : ServiceLocatorException(fallback_translate("FactoryException", "what", QStringLiteral(
            "An error occurred in a factory.\n"
            "    factoryProductName: \"%1\"\n"
            "           factoryName: \"%2\"\n"
            "              why code: %3"))
            .arg(factoryProductName)
            .arg(factoryName)
            .arg(why)),
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

#endif // SERVICELOCATOREXCEPTIONS_HPP