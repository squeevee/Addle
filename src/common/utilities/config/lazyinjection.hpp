#pragma once

#include <memory>
#include <functional>

#include <boost/di.hpp>

#include "interfaces/services/ifactory.hpp"
#include "./storedparameters.hpp"

namespace Addle {

template<typename Interface>
class LazyInjection
{
    template<typename Interface_>
    using _no_arg_make_t = decltype( std::declval<const IFactory<Interface_>&>().make() );
    using factory_permits_no_args = boost::is_detected<_no_arg_make_t, Interface>;
    
public:
    using boost_di_inject__ = boost::di::inject<const IFactory<Interface>&>;
    LazyInjection(const IFactory<Interface>& factory)
        : _factory(factory)
    {
        if constexpr (factory_permits_no_args::value)
        {
            const IFactory<Interface>& factory = _factory;
            _initializer = [&factory] () {
                    return factory.makeUnique();
                };
        }
    }
    
    template<typename... Args>
    inline LazyInjection& bind(Args&&... args)
    {
        const IFactory<Interface>& factory = _factory;
        auto params = config_detail::make_stored_parameters(std::forward<Args>(args)...);
        _initializer = [ params{ std::move(params) }, &factory ] () {
                return std::unique_ptr<Interface>(std::move(params).applyToMake(factory));
            };
            
        return *this;
    }
    
    inline const Interface& get() const 
    {
        lazyInitialize();
        return *_instance;
    }
    
    inline QSharedPointer<Interface> asShared() &&
    {
        lazyInitialize();
        return QSharedPointer<Interface>(_instance.release());
    }
    
    inline std::unique_ptr<Interface> asUnique() &&
    {
        lazyInitialize();
        return std::move(_instance);
    }
    
    explicit inline operator QSharedPointer<Interface>() && { return asShared(); }
    explicit inline operator std::unique_ptr<Interface>() && { return asUnique(); }
    explicit inline operator const Interface& () const { return get(); }
    
    inline void clear()
    {
        _instance.reset();
        _initializer = nullptr;
    }
    
private:
    void lazyInitialize() const
    {
        if (!_instance)
        {
            // assert _initializer
            _instance = _initializer();
        }
    }
    
    mutable std::unique_ptr<Interface> _instance = nullptr;
    
    std::function<std::unique_ptr<Interface>()> _initializer;
    const IFactory<Interface>& _factory;
};

} // namespace Addle
