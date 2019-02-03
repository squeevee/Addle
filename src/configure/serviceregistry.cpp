#include "configure/serviceregistry.h"

#include <unordered_map>
#include <string>

IService* ServiceRegistry::getService(const char* name)
{
    return _instance->_registry[name];
}

ServiceRegistry::~ServiceRegistry()
{
    for (const auto& service : _registry )
    {
        delete service.second;
    }
}