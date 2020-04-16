#ifndef IFACTORY_HPP
#define IFACTORY_HPP
 
#include <typeinfo>

#include "iservicelocator.hpp"
#include "interfaces/traits/compat.hpp"

class ADDLE_COMMON_EXPORT IFactory
{
public:
    virtual ~IFactory() = default;
    virtual void* make() const = 0;
    virtual void delete_(void* obj) const = 0;
};

#endif // IFACTORY_HPP
