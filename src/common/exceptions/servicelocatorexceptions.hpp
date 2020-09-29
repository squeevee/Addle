/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef SERVICELOCATOREXCEPTIONS_HPP
#define SERVICELOCATOREXCEPTIONS_HPP

#include "addleexception.hpp"
#include "idtypes/addleid.hpp"

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
            qtTrId("debug-messages.service-locator.not-initialized-error")
        )
    {
    }
#else
    ServiceLocatorNotInitializedException() = default;
#endif

    virtual ~ServiceLocatorNotInitializedException() = default;
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
            const char* requestedInterfaceName
        )
        : ServiceLocatorException(
            //% "No factory was found for the interface \"%1\"."
            qtTrId("debug-messages.service-locator.factory-by-type-not-found-error")
                .arg(requestedInterfaceName)
        ),
        _requestedInterfaceName(requestedInterfaceName)
    {
    }
    FactoryNotFoundException(
            const char* requestedInterfaceName,
            AddleId id
        )
        : ServiceLocatorException(
            //% "No factory was found for the interface \"%1\" and the ID \"%2\"."
            qtTrId("debug-messages.service-locator.factory-by-id-not-found-error")
                .arg(requestedInterfaceName)
                .arg(id.key())
        ),
        _requestedInterfaceName(requestedInterfaceName),
        _id(id)
    {
    }
private:
    const char* _requestedInterfaceName;
    const AddleId _id;
#else
public:
    FactoryNotFoundException() = default;
#endif
public:
    virtual ~FactoryNotFoundException() = default;
};

} // namespace Addle

#endif // SERVICELOCATOREXCEPTIONS_HPP
