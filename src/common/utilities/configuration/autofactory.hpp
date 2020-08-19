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

#include "interfaces/ifactory.hpp"
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

    virtual void* make() const
    { 
        return reinterpret_cast<void*>(
            static_cast<Interface*>(new Impl)
        );
    }

    void delete_(void* obj) const 
    {
        delete reinterpret_cast<Interface*>(obj);
    }
};

// Interfaces implemented as QObject may desire some QObject-aware logic in
// their AutoFactory, for example with regard to thread affinity. Keep this in
// mind for the future.

#define CONFIG_AUTOFACTORY_BY_TYPE(Interface, Impl) \
ServiceConfigurationBase::registerFactoryByType<Interface>(new AutoFactory<Interface, Impl>());

#define CONFIG_AUTOFACTORY_BY_ID(Interface, id, Impl) \
ServiceConfigurationBase::registerFactoryById<Interface>(new AutoFactory<Interface, Impl>(), id);

} // namespace Addle
#endif // AUTOFACTORY_HPP
