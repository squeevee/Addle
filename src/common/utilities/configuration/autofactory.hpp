/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef AUTOFACTORY_HPP
#define AUTOFACTORY_HPP

#include <type_traits>

#include "servicelocator.hpp"

#include "interfaces/traits.hpp"
#include "utilities/errors.hpp"

#include "interfaces/config/ifactory.hpp"

namespace Addle {

/**
 * A generic template-based object factory.
 */
template<class Interface, class Impl>
class AutoFactory : public IFactory
{
    static_assert(
        Traits::is_makeable<Interface>::value,
        "Interface must be makeable"
    );

public:
    virtual ~AutoFactory() = default;

    virtual void* make(AddleId id = AddleId()) const 
    {
        //ADDLE_ASSERT(!id);
        return reinterpret_cast<void*>(
            static_cast<Interface*>(new Impl)
        );
    }

    void delete_(void* obj) const 
    {
        delete reinterpret_cast<Interface*>(obj);
    }
};

template<class Interface, class Impl>
class AutoFactoryById : public IFactory
{
    static_assert(
        Traits::is_makeable<Interface>::value,
        "Interface must be makeable"
    );
    
public:
    virtual ~AutoFactoryById() = default;

    virtual void* make(AddleId id = AddleId()) const 
    {
        return reinterpret_cast<void*>(
            static_cast<Interface*>(new Impl(id))
        );
    }

    void delete_(void* obj) const 
    {
        delete reinterpret_cast<Interface*>(obj);
    }
};

} // namespace Addle

#endif // AUTOFACTORY_HPP
