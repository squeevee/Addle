#ifndef TFACTORY_H
#define TFACTORY_H

#include <type_traits>

#include "interfaces/services/iservicelocator.h"
#include "interfaces/services/ifactory.h"
#include "interfaces/services/ifactoryproduct.h"

/**
 * A generic template-based object factory.
 */
template<class Interface, class Concrete>
class TFactory : public IFactory
{
    static_assert(std::is_base_of<Interface, Concrete>::value,
        "Concrete must inherit Interface");

public:
    virtual ~TFactory() = default;

    IFactoryProduct* make();

    const char* getFactoryName()
    {
        return typeid(TFactory<Interface, Concrete>).name();
    }

    void setServiceLocator(IServiceLocator* serviceLocator) { _serviceLocator = serviceLocator; }

private:
    IServiceLocator* _serviceLocator;
};

/**
 * A generic template-based factory product
 */
template<class Interface, class Concrete>
class TFactoryProduct : public Concrete, public IFactoryProduct
{
    static_assert(std::is_base_of<Interface, Concrete>::value,
        "Concrete must inherit Interface");

public:
    TFactoryProduct(TFactory<Interface, Concrete>* factory)
        : _factory(factory)
    {
    }
    
    virtual ~TFactoryProduct() = default;
    
    IFactory* getFactory() { return _factory; }
    const char* getFactoryProductName()
    {
        return typeid(TFactoryProduct<Interface, Concrete>).name();
    }
    
private:
    IFactory* _factory;
};


template<class Interface, class Concrete>
IFactoryProduct* TFactory<Interface, Concrete>::make()
{
    return new TFactoryProduct<Interface, Concrete>(this);
}

// Convenience macro for use inside
// ServiceLocator::AbstractConfiguration::configure() to register a TFactory
// to the given interface.
#define REGISTER_TFACTORY(Interface, Concrete) \
    registerFactory<Interface, TFactory<Interface, Concrete>>()

#endif //TFACTORY_H
