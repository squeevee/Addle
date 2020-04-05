#ifndef IFACTORY_HPP
#define IFACTORY_HPP
 
#include <typeinfo>

#include "iservicelocator.hpp"

class IFactory
{
public:
    virtual ~IFactory() = default;
    virtual void* make() const = 0;
    virtual void delete_(void* obj) const = 0;
};

#endif // IFACTORY_HPP
