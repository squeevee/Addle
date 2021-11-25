#pragma once

#include <functional>
#include <typeinfo>
#include <typeindex>

#include <boost/range/adaptor/transformed.hpp>

#include <QMap>
#include <QHash>

#include <QReadWriteLock>
#include <QObject>

#include "./aux.hpp"
#include "./bindingcondition.hpp"
#include "./dynamicservicestorage.hpp"

#include "utilities/hashfunctions.hpp"

namespace Addle {
namespace aux_config3 {

template<class RuntimeBindings>
struct di_runtime_bindings_traits;

enum MakeFormat { Plain, Shared };

struct MakeBinding
{
    using function_t = void (
              void* // output
            , void* // injector
            , void* // parameter set
            , MakeFormat
        );
    
    function_t* function    = nullptr;
    void* injector          = nullptr;
    
    const std::type_info* interfaceType = nullptr;
    
#ifdef ADDLE_DEBUG
    const std::type_info* implType = nullptr;
#endif
    
    BindingCondition condition; 
    
    template<typename Interface, std::enable_if_t<config_detail::has_factory_params<Interface>::value, void*> = nullptr>
    Interface* make(config_detail::factory_params_t<Interface>&& params) const
    {
        assert(function && injector);
#ifdef ADDLE_DEBUG
        assert(interfaceType && *interfaceType == typeid(Interface));
#endif
        Interface* result;
        function(&result, injector, std::addressof(params), Plain);
        return result;
    }
    
    template<typename Interface, std::enable_if_t<!config_detail::has_factory_params<Interface>::value, void*> = nullptr>
    Interface* make() const
    {
        assert(function && injector);
#ifdef ADDLE_DEBUG
        assert(interfaceType && *interfaceType == typeid(Interface));
#endif
        Interface* result;
        function(&result, injector, nullptr, Plain);
        return result;
    }
    
    template<typename Interface, std::enable_if_t<config_detail::has_factory_params<Interface>::value, void*> = nullptr>
    QSharedPointer<Interface> makeShared(config_detail::factory_params_t<Interface>&& params) const
    {
#ifdef ADDLE_DEBUG
        assert(interfaceType && *interfaceType == typeid(Interface));
#endif
        assert(function && injector);
        
        QSharedPointer<Interface> result;
        function(&result, injector, std::addressof(params), Shared);
        return result;
    }
    
    template<typename Interface, std::enable_if_t<!config_detail::has_factory_params<Interface>::value, void*> = nullptr>
    QSharedPointer<Interface> makeShared() const
    {
#ifdef ADDLE_DEBUG
        assert(interfaceType && *interfaceType == typeid(Interface));
#endif
        assert(function && injector);
        
        QSharedPointer<Interface> result;
        function(&result, injector, nullptr, Shared);
        return result;
    }
};

struct GetBinding
{
    using function_t = void*(void*);
    
    function_t* function    = nullptr;
    void* injector          = nullptr;
    
    const std::type_info* interfaceType = nullptr;
    
#ifdef ADDLE_DEBUG
    const std::type_info* implType = nullptr;
#endif
    
    template<typename Interface>
    Interface* get() const
    {
#ifdef ADDLE_DEBUG
        assert(interfaceType && *interfaceType == typeid(Interface));
#endif
        assert(function && injector);
        return reinterpret_cast<Interface*>(function(injector));
    }
};

class ModuleConfig;
class InterfaceBindingConfig : public QObject
{
    Q_OBJECT
public:
    InterfaceBindingConfig() = default;
    virtual ~InterfaceBindingConfig() = default;
    
    ModuleConfig addModule(QByteArray moduleId);
    
private:
    struct ModuleEntry
    {
        QByteArray id;
        std::vector<anonymous_unique_ptr> injectors;
        QList<MakeBinding> makeBindings;
        QList<GetBinding> getBindings;
    };
    
    struct InterfaceEntry
    {
        const std::type_info& type;
        
        QList<MakeBinding> conditionalMakes;
        
#ifdef ADDLE_DEBUG
        template<typename T> using make_index_t = QMultiMap<T, MakeBinding>;
#else
        template<typename T> using make_index_t = QMultiHash<T, MakeBinding>;
#endif
        
        std::variant<
              std::monostate
            , make_index_t<int>
            , make_index_t<unsigned>
            , make_index_t<long>
            , make_index_t<unsigned long>
            , make_index_t<QByteArray>
        >
        makeIndex;
        int conditionalMakeIndexedParameter = -1;
        
        MakeBinding unconditionalMake;
        
        GetBinding get;
    };
    
    template<typename Params>
    struct _make_index_visitor
    {
        using value_types = boost::mp11::mp_transform<boost::remove_cv_ref_t, typename Params::value_types>;
        
        const MakeBinding* operator()(std::monostate) const { return nullptr; }
        
        template<typename T>
        const MakeBinding* operator()(const InterfaceEntry::make_index_t<T>& makeIndex) const;
        
        const MakeBinding* operator()(const InterfaceEntry::make_index_t<QByteArray>& makeIndex) const;
        
        const Params& params;
        int indexedParameter;
    };

    template<typename Interface, std::enable_if_t<config_detail::has_factory_params<Interface>::value, void*> = nullptr>
    const MakeBinding* findMakeBinding(const config_detail::factory_params_t<Interface>& params, bool silenceErrors = false) const;
    
    template<typename Interface, std::enable_if_t<!config_detail::has_factory_params<Interface>::value, void*> = nullptr>
    const MakeBinding* findMakeBinding(bool silenceErrors = false) const;
    
    template<typename Interface>
    Interface* getService(bool silenceErrors = false) const;
    
    template<typename Interface, typename F>
    Interface* initializeService(F&& f) const;
    
    template<typename Interface, std::enable_if_t<config_detail::has_factory_params<Interface>::value, void*> = nullptr>
    Interface* make_p(config_detail::factory_params_t<Interface>&& params) const
    {
        const QReadLocker lock(&_lock);
        return findMakeBinding<Interface>(params)->template make<Interface>(std::move(params));
    }
    
    template<typename Interface, std::enable_if_t<!config_detail::has_factory_params<Interface>::value, void*> = nullptr>
    Interface* make_p() const
    {
        const QReadLocker lock(&_lock);
        return findMakeBinding<Interface>()->template make<Interface>();
    }
    
    template<typename Interface, std::enable_if_t<config_detail::has_factory_params<Interface>::value, void*> = nullptr>
    QSharedPointer<Interface> makeShared_p(config_detail::factory_params_t<Interface>&& params) const
    {
        const QReadLocker lock(&_lock);
        return findMakeBinding<Interface>(params)->template makeShared<Interface>(std::move(params));
    }
    
    template<typename Interface, std::enable_if_t<!config_detail::has_factory_params<Interface>::value, void*> = nullptr>
    QSharedPointer<Interface> makeShared_p() const
    {
        const QReadLocker lock(&_lock);
        return findMakeBinding<Interface>()->template makeShared<Interface>();
    }
    
    std::list<ModuleEntry> _modules;
    QHash<QByteArray, std::list<ModuleEntry>::iterator> _index_byModuleId;
    
    std::list<InterfaceEntry> _interfaces;
    QHash<std::type_index, std::list<InterfaceEntry>::iterator> _index_byInterface;
    
    mutable DynamicServiceStorage _dynamicServices;
    
    mutable QReadWriteLock _lock;
    
    friend class ModuleConfig;
    friend struct di_runtime_bindings_traits<InterfaceBindingConfig>;
};

template<typename Params>
template<typename T>
const MakeBinding* InterfaceBindingConfig::_make_index_visitor<Params>::operator()
    (const InterfaceEntry::make_index_t<T>& makeIndex) const
{
    assert(indexedParameter >= 0 && indexedParameter < boost::mp11::mp_size<Params>::value);
    assert(( test_static_predicate<_integral_alias_q<T>::template fn, value_types>(indexedParameter) ));
    
    auto anonParams = _get_anonymous_params(params);
    auto number = *reinterpret_cast<const T*>(anonParams[indexedParameter]);
    
    auto find = makeIndex.find(number);
    return Q_LIKELY(find != makeIndex.end()) ? &(*find) : nullptr;
}
        
template<typename Params>
const MakeBinding* InterfaceBindingConfig::_make_index_visitor<Params>::operator()
    (const InterfaceEntry::make_index_t<QByteArray>& makeIndex) const
{
    assert(indexedParameter >= 0 && indexedParameter < boost::mp11::mp_size<Params>::value);
    
    QByteArray bytes = _bind_condition_bytes_extract_impl<value_types> 
        { _get_anonymous_params(params).data(), (std::size_t)indexedParameter } ();
        
    auto find = makeIndex.find(bytes);
    return Q_LIKELY(find != makeIndex.end()) ? &(*find) : nullptr;
}
        
template<typename Interface, std::enable_if_t<config_detail::has_factory_params<Interface>::value, void*>>
const MakeBinding* InterfaceBindingConfig::findMakeBinding(
        const config_detail::factory_params_t<Interface>& params, 
        bool silenceErrors) const
{
    auto find = noDetach(_index_byInterface).find(typeid(Interface));
    
    if (Q_UNLIKELY(find == _index_byInterface.cend()))
    {
        if (!silenceErrors)
        {
            ADDLE_THROW(InterfaceBindingNotFoundException(
                    InterfaceBindingNotFoundException::NotBound, typeid(Interface)
                ));
        }
        else
        {
            return nullptr;
        }
    }
    
    const auto& entry = **find;
    
    if (std::holds_alternative<std::monostate>(entry.makeIndex))
    {
        for (const auto& binding : noDetach(entry.conditionalMakes))
        {
            if (binding.condition.test(params))
                return &binding;
        }
    }
    else
    {
        auto found = std::visit(
            _make_index_visitor<config_detail::factory_params_t<Interface>> 
                { params, entry.conditionalMakeIndexedParameter }, 
            entry.makeIndex
        );
        
        if (Q_LIKELY(found)) 
        {
#ifdef ADDLE_DEBUG
            assert(found->condition.test(params));
#endif  
            return found;
        }
    }
    
    if (Q_UNLIKELY(!entry.unconditionalMake.function))
    {
        if (!silenceErrors)
        {
            ADDLE_THROW(InterfaceBindingNotFoundException(
                    InterfaceBindingNotFoundException::ConditionFailure
                    , typeid(Interface)
#ifdef ADDLE_DEBUG
                    , qToList(
                        noDetach(entry.conditionalMakes)
                        | boost::adaptors::transformed([](const auto& binding) {
                            return QSharedPointer<BindingCondition>::create(binding.condition);
                        }))
                    , [params] () -> QString { return aux_debug::debugString(params); }
#endif
                ));
        }
        else
        {
            return nullptr;
        }
    }
    
    return &entry.unconditionalMake;
}

template<typename Interface, std::enable_if_t<!config_detail::has_factory_params<Interface>::value, void*>>
const MakeBinding* InterfaceBindingConfig::findMakeBinding(bool silenceErrors) const
{
    auto find = noDetach(_index_byInterface).find(typeid(Interface));
    
    if (Q_UNLIKELY(find == _index_byInterface.cend()))
    {
        if (!silenceErrors)
        {
            ADDLE_THROW(InterfaceBindingNotFoundException(
                InterfaceBindingNotFoundException::NotBound, typeid(Interface)
            ));
        }
        else
        {
            return nullptr;
        }
    }
    
    assert((**find).unconditionalMake.function);
    return &(**find).unconditionalMake;
}

template<typename Interface> 
Interface* InterfaceBindingConfig::getService(bool silenceErrors) const
{
    const QReadLocker lock(&_lock);
    
    auto find = _index_byInterface.find(typeid(Interface));
    if (Q_UNLIKELY(find == _index_byInterface.end()))
    {
         if (!silenceErrors)
        {
            ADDLE_THROW(InterfaceBindingNotFoundException(
                InterfaceBindingNotFoundException::NotBound, typeid(Interface)
            ));
        }
        else
        {
            return nullptr;
        }
    }
    
    return (**find).get.get<Interface>();
}

template<typename Interface, typename F> 
Interface* InterfaceBindingConfig::initializeService(F&& f) const
{
    const QReadLocker lock(&_lock);
    
    Interface* result;
    std::tie(result, std::ignore) = _dynamicServices.initializeService<Interface>(std::forward<F>(f));
    
    assert(result);
    return result;
}

template<> struct di_runtime_bindings_traits<InterfaceBindingConfig>
{
    template<typename Interface, std::enable_if_t<!config_detail::has_factory_params<Interface>::value, void*> = nullptr> 
    static Interface* make(const InterfaceBindingConfig* ic)
    {
        assert(ic); return ic->make_p<Interface>();
    }
    
    template<typename Interface, std::enable_if_t<config_detail::has_factory_params<Interface>::value, void*> = nullptr> 
    static Interface* make(const InterfaceBindingConfig* ic, config_detail::factory_params_t<Interface>&& params)
    {
        assert(ic); return ic->make_p<Interface>(std::move(params));
    }

    template<typename Interface, std::enable_if_t<!config_detail::has_factory_params<Interface>::value, void*> = nullptr> 
    static QSharedPointer<Interface> make_shared(const InterfaceBindingConfig* ic)
    {
        assert(ic); return ic->makeShared_p<Interface>();
    }
    
    template<typename Interface, std::enable_if_t<config_detail::has_factory_params<Interface>::value, void*> = nullptr> 
    static QSharedPointer<Interface> make_shared(const InterfaceBindingConfig* ic, config_detail::factory_params_t<Interface>&& params)
    {
        assert(ic); return ic->makeShared_p<Interface>(std::move(params));
    }

    template<typename Interface> 
    static Interface* get_service(const InterfaceBindingConfig* ic)
    {
        assert(ic); return ic->getService<Interface>();
    }
    
    template<typename Interface, typename F> 
    static Interface* get_or_initialize_service(const InterfaceBindingConfig* ic, F&& f)
    {
        assert(ic);
        Interface* result;
        return (
               (result = ic->getService<Interface>(true)) 
            || (result = ic->initializeService<Interface>(std::forward<F>(f)))
            , result
        );
    }
};

}

}
