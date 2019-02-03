#ifndef CONFIGURESERVICEREGISTRY_H
#define CONFIGURESERVICEREGISTRY_H

#include "interfaces/services/iservice.h"
#include "configure/serviceregistry.h"

namespace configure
{
    class ConfigureServiceRegistry
    {
    public:
        static void initialize();
        static void registerService(const char* name, IService* concrete);
        static void destroy();
    };
}

#define REGISTER_SERVICE(service_interface, concrete) configure::ConfigureServiceRegistry::registerService(#service_interface, concrete);

#endif //CONFIGURESERVICEREGISTRY_H