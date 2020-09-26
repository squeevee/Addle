/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IFACTORY_HPP
#define IFACTORY_HPP
 
#include <typeinfo>
#include "idtypes/addleid.hpp"

namespace Addle {

class IFactory
{
public:
    virtual ~IFactory() = default;
    virtual void* make(AddleId id = AddleId()) const = 0;
    virtual void delete_(void* obj) const = 0;
    
#ifdef ADDLE_DEBUG
    virtual const char* factoryName() const = 0;
    virtual const char* implementationName() const = 0;
#endif
};
} // namespace Addle

#endif // IFACTORY_HPP
