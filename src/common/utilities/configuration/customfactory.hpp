#ifndef CUSTOMFACTORY_HPP
#define CUSTOMFACTORY_HPP

#include <functional>
#include <memory>

#include "interfaces/servicelocator/ifactory.hpp"

template<class Interface>
class CustomFactory : public IFactory
{
public:
    CustomFactory(
        std::function<Interface*()> maker,
        std::function<void(Interface*)> deleter = std::default_delete<Interface>()
    )
        : _maker(maker), _deleter(deleter)
    {
    }

    virtual ~CustomFactory() = default;

    void* make() const 
    {
        return reinterpret_cast<void*>(_maker());
    }

    void delete_(void* ptr) const
    {
        _deleter(reinterpret_cast<Interface*>(ptr));
    }

private:
    const std::function<Interface*()> _maker;
    const std::function<void(Interface*)> _deleter;
};

#define CONFIG_STATIC_CUSTOMFACTORY(Interface, ...) \
template<> const IFactory* const StaticFactoryLocator<Interface>::factory = new CustomFactory<Interface>(__VA_ARGS__);

#define CONFIG_DYNAMIC_CUSTOMFACTORY(Interface, id, ...) \
BaseServiceConfiguration::registerDynamicFactory<Interface>(new CustomFactory<Interface>(__VA_ARGS__), id);

#endif // CUSTOMFACTORY_HPP