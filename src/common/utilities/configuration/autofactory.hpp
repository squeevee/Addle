#ifndef AUTOFACTORY_HPP
#define AUTOFACTORY_HPP

#include <type_traits>

#include "servicelocator.hpp"

#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/by_id_traits.hpp"
#include "interfaces/servicelocator/iservicelocator.hpp"
#include "interfaces/servicelocator/ifactory.hpp"

/**
 * A generic template-based object factory.
 */
template<class Interface, class Impl>
class AutoFactory : public IFactory
{
    static_assert(
        is_makeable<Interface>::value || is_makeable_by_id<Interface>::value,
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

#define CONFIG_STATIC_AUTOFACTORY(Interface, Impl) \
template<> const IFactory* const StaticFactory<Interface>::instance = new AutoFactory<Interface, Impl>();

#define CONFIG_DYNAMIC_AUTOFACTORY(Interface, id, Impl) \
BaseServiceConfiguration::registerDynamicFactory<Interface>(new AutoFactory<Interface, Impl>(), id);

#endif // AUTOFACTORY_HPP
