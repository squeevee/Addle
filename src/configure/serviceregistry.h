#ifndef SERVICEREGISTRY_H
#define SERVICEREGISTRY_H

#include <unordered_map>
#include <string>

#include "interfaces/services/iservice.h"

namespace configure
{
    class ConfigureServiceRegistry;
};

class ServiceRegistry
{
public:
    static IService* getService(const char* name);

private:
    ServiceRegistry() {}
    virtual ~ServiceRegistry();

    std::unordered_map<std::string, IService*> _registry;

    static ServiceRegistry* _instance;

    friend class configure::ConfigureServiceRegistry;
};

#define GET_SERVICE(service_interface) dynamic_cast<service_interface*>(ServiceRegistry::getService(#service_interface))

#endif //SERVICEREGISTRY_H