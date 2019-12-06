#ifndef TFACTORY_HPP
#define TFACTORY_HPP

#include <type_traits>

#include "interfaces/servicelocator/iservicelocator.hpp"
#include "interfaces/servicelocator/ifactory.hpp"

/**
 * A generic template-based object factory.
 */
template<class Interface, class Impl>
class TFactory : public virtual IFactory
{
    static_assert(
        std::is_base_of<IMakeable, Interface>::value,
        "Interface must implement IMakeable"
    );

public:
    virtual ~TFactory() = default;

    IMakeable* make() { return static_cast<IMakeable*>(new Impl); }

#ifdef ADDLE_DEBUG
    const char* getFactoryTypeName()
    {
        static const char* factoryName = typeid(TFactory<Interface, Impl>).name();
        return factoryName;
    }
    const char* getProductTypeName()
    {
        static const char* productName = typeid(Impl).name();
        return productName;
    }
#endif
    void setServiceLocator(IServiceLocator* serviceLocator) { _serviceLocator = serviceLocator; }

protected:
    IServiceLocator* _serviceLocator;
};

// /**
//  * A generic template-based factory product
//  */
// template<class Impl>
// class TFactoryProduct : public Impl, public virtual IFactoryProduct
// {
// public:
//     virtual ~TFactoryProduct() = default;
    
// #ifdef ADDLE_DEBUG
//     static const char* staticFactoryName()
//     {
//         return typeid(TFactory<Impl>).name();
//     }
//     static const char* staticFactoryProductName()
//     {
//         return typeid(TFactoryProduct<Impl>).name();
//     }

//     const char* getFactoryName()
//     { 
//         return staticFactoryName();
//     }

//     const char* getFactoryProductName()
//     {
//         return staticFactoryProductName();
//     }
// #endif
// };


// template<class Impl>
// IFactoryProduct* TFactory<Impl>::make()
// {
//     return new TFactoryProduct<Impl>();
// }

// Convenience macro for use inside
// BaseServiceConfiguration::configure() to register a TFactory
// to the given interface.
#define REGISTER_TFACTORY(Interface, Impl) \
    BaseServiceConfiguration::registerFactory<Interface, TFactory<Interface, Impl>, Impl>();

#endif // TFACTORY_HPP
