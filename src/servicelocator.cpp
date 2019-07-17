/**
 * Addle source code
 * Copyright 2019 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#include "servicelocator.h"

void ServiceLocator::AbstractConfiguration::initialize()
{
    if (ServiceLocator::_instance)
        throw ServiceLocator::ServiceLocatorAlreadyInitializedException();

    ServiceLocator::_instance = new ServiceLocator();
    configure();
}


void ServiceLocator::AbstractConfiguration::destroy()
{
    if (!ServiceLocator::_instance)
        throw ServiceLocator::ServiceLocatorNotInitializedException();

    delete ServiceLocator::_instance;
    ServiceLocator::_instance = nullptr;
}

ServiceLocator::ServiceLocatorNotInitializedException::ServiceLocatorNotInitializedException()
    : std::logic_error("The service locator is not initialized.")
{
}

ServiceLocator::ServiceLocatorAlreadyInitializedException::ServiceLocatorAlreadyInitializedException()
    : std::logic_error("The service locator has already been initialized.")
{
}

ServiceLocator::FactoryNotFoundException::FactoryNotFoundException(const char* requestedInterfaceName, int factoryCount)
    : _requestedInterfaceName(requestedInterfaceName),
        _factoryCount(factoryCount),
        std::logic_error("No factory was found for the requested interface.")
{
}

ServiceLocator::ServiceNotFoundException::ServiceNotFoundException(const char* requestedInterfaceName, int serviceCount, int factoryCount)
    : _requestedInterfaceName(requestedInterfaceName),
        _serviceCount(serviceCount),
        _factoryCount(factoryCount),
        std::logic_error("No service was found for the requested interface.")
{
}

ServiceLocator::InvalidFactoryProductException::InvalidFactoryProductException(const char* requestedInterfaceName, const char* factoryProductName, const char* factoryName)
    : _requestedInterfaceName(requestedInterfaceName),
    _factoryProductName(factoryProductName),
    _factoryName(factoryName),
    std::logic_error("An invalid factory product was created.")
{
}

ServiceLocator::~ServiceLocator()
{
    for (auto& pair : _factoryregistry)
    {
        delete pair.second;
    }
    
    for (auto& pair : _serviceregistry)
    {
        delete pair.second;
    }
}

ServiceLocator* ServiceLocator::_instance = nullptr;