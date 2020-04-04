#ifndef TFACTORY_HPP
#define TFACTORY_HPP

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
class TFactory : public IFactory
{
    static_assert(
        is_makeable<Interface>::value || is_makeable_by_id<Interface>::value,
        "Interface must be makeable"
    );

public:

    virtual ~TFactory() = default;

    virtual void* make() const
    { 
        return reinterpret_cast<void*>(
            static_cast<Interface*>(new Impl)
        );
    }

    void destroy(void* obj) const 
    {
        delete reinterpret_cast<Interface*>(obj);
    }
};

#define DEFINE_STATIC_FACTORY(Interface, Impl) \
template<> const IFactory* const StaticFactory<Interface>::instance = new TFactory<Interface, Impl>();

#define REGISTER_TFACTORY(Interface, Impl, id) \
BaseServiceConfiguration::registerDynamicFactory<Interface, Impl>(new TFactory<Interface, Impl>(), id);

#endif // TFACTORY_HPP
