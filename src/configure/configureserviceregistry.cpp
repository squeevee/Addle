#include "configure/configureserviceregistry.h"

using namespace configure;

ServiceRegistry* ServiceRegistry::_instance;

void ConfigureServiceRegistry::initialize()
{
    ServiceRegistry::_instance = new ServiceRegistry();
}

void ConfigureServiceRegistry::registerService(const char* name, IService* concrete)
{
    ServiceRegistry::_instance->_registry[name] = concrete;
}

void ConfigureServiceRegistry::destroy()
{
    delete ServiceRegistry::_instance;
    ServiceRegistry::_instance = nullptr;
}