#include "servicelocator.hpp"

ServiceLocator* ServiceLocator::_instance = nullptr;

ServiceLocator::~ServiceLocator()
{
    for (auto factory : _factoryregistry)
    {
        delete factory;
    }
    
    for (auto service : _serviceregistry)
    {
        delete service;
    }
}
