#ifndef INJECTOR_HPP
#define INJECTOR_HPP

#include <functional>
#include <list>
#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <memory>

#include <boost/any.hpp>

#include <QHash>

#include "idtypes/addleid.hpp"

#include "interfaces/services/ifactory.hpp"
#include "interfaces/services/irepository.hpp"

#include "utilities/qobject.hpp"

#include "bindingfilter.hpp"
#include "bindingselector.hpp"

#include "bindingexception.hpp"

#include "staticbinding.hpp"

namespace Addle { namespace Config {
namespace detail {
    
template<class Interface>
using _normalize_typename = typename std::remove_cv<
        typename std::remove_reference<Interface>::type
    >::type;

template<class N, class Interface>
using _is_init_param_test = typename std::integral_constant<bool,
        N::value < std::tuple_size<init_params_t<Interface>>::value
    >::type;
        
template<std::size_t N, class Interface>
using _is_init_param = typename boost::detected_or<
        std::false_type, _is_init_param_test, std::integral_constant<std::size_t, N>, Interface
    >::type;

template<std::size_t N, class Interface>
using _init_param_element_t = typename std::tuple_element<N, init_params_t<Interface>>::type;

template<std::size_t N, class Interface, typename ArgT>
using _is_convertible_init_param = typename std::conditional<
        _is_init_param<N, Interface>::value,
        typename std::is_convertible<_init_param_element_t<N, Interface>, ArgT>,
        std::false_type
    >::type;
    
template<bool, std::size_t N, class Interface>
struct _lvalue_reference_param_test_impl : std::false_type {};

template<std::size_t N, class Interface>
struct _lvalue_reference_param_test_impl<true, N, Interface> : 
    std::is_lvalue_reference<_init_param_element_t<N, Interface>> {};

template<class N, class Interface>
using _lvalue_reference_param_test = _lvalue_reference_param_test_impl<
        N::value < std::tuple_size<init_params_t<Interface>>::value,
        N::value, Interface
    >;

template<std::size_t N, class Interface>
using _is_lvalue_reference_param = typename boost::detected_or<
        std::false_type, _lvalue_reference_param_test, std::integral_constant<std::size_t, N>, Interface
    >::type;

template<std::size_t N, class Interface, typename ArgT>
using _is_lvalue_reference_arg = typename std::integral_constant<bool,
        Traits::is_singleton_gettable<_normalize_typename<ArgT>>::value
        || _is_lvalue_reference_param<N, Interface>::value
    >;

template<bool, std::size_t N, class Interface>
struct _rvalue_reference_param_test_impl : std::false_type {};

template<std::size_t N, class Interface>
struct _rvalue_reference_param_test_impl<true, N, Interface> : 
    std::is_rvalue_reference<_init_param_element_t<N, Interface>> {};

template<class N, class Interface>
using _rvalue_reference_param_test = _rvalue_reference_param_test_impl<
        N::value < std::tuple_size<init_params_t<Interface>>::value,
        N::value, Interface
    >;

template<std::size_t N, class Interface>
using _is_rvalue_reference_param = typename boost::detected_or<
        std::false_type, _rvalue_reference_param_test, std::integral_constant<std::size_t, N>, Interface
    >::type;

template<std::size_t N, class Interface, typename ArgT>
using _is_rvalue_reference_arg = typename std::integral_constant<bool,
        _is_rvalue_reference_param<N, Interface>::value
    >;
    
template<class Interface>
using _required_arg_count = std::integral_constant<std::size_t, 0>;/*typename std::conditional<
        has_init_params<Interface>::value,
        std::tuple_size<init_params_t<Interface>>,
        std::integral_constant<std::size_t, 0>
    >::type;*/
    

template<typename To, typename From>
struct _parameter_converter
{
    To operator()(From) const = delete;
};

template<typename To, typename From>
struct _parameter_converter<To&, From&>
{
    To& operator()(From& from) const
    {
        return convert_impl<To, From>(from);
    }
    
    template<typename To_, typename From_>
    typename std::enable_if<
        std::is_base_of<QObject, _normalize_typename<From_>>::value,
        To_&
    >::type convert_impl(From_& from) const
    {
        To_* result = qobject_cast<To_*>(&from);
        ADDLE_ASSERT(result);
        return *result;
    }
    
    template<typename To_, typename From_>
    typename std::enable_if<
        Traits::implemented_as_QObject<_normalize_typename<From_>>::value,
        To_&
    >::type convert_impl(From_& from) const
    {
        To_* result = qobject_interface_cast<To_*>(&from);
        ADDLE_ASSERT(result);
        return *result;
    }
};

template<typename To, typename From>
struct _parameter_converter<To*, From*>
{
    To* operator()(From* from) const
    {
        return convert_impl<To, From>(from);
    }
    
    template<typename To_, typename From_>
    typename std::enable_if<
        std::is_base_of<QObject, _normalize_typename<From_>>::value,
        To_*
    >::type convert_impl(From_* from) const
    {
        return qobject_cast<To*>(from);
    }
    
    template<typename To_, typename From_>
    typename std::enable_if<
        Traits::implemented_as_QObject<_normalize_typename<From_>>::value,
        To_*
    >::type convert_impl(From_* from) const
    {
        return qobject_interface_cast<To*>(from);
    }
};

template<typename To, typename From>
struct _parameter_converter<QSharedPointer<To>, QSharedPointer<From>>
{
    QSharedPointer<To> operator()(QSharedPointer<From> from) const
    {
        return convert_impl<To, From>(from);
    }
    
    template<typename To_, typename From_>
    typename std::enable_if<
        std::is_base_of<QObject, _normalize_typename<From_>>::value,
        QSharedPointer<To>
    >::type convert_impl(QSharedPointer<From> from) const
    {
        return from.template objectCast<To>();
    }
    
    template<typename To_, typename From_>
    typename std::enable_if<
        Traits::implemented_as_QObject<_normalize_typename<From_>>::value,
        QSharedPointer<To>
    >::type convert_impl(QSharedPointer<From> from) const
    {
        return qobject_interface_cast<To>(from);
    }
};

    
class Container;

// TODO: The current implementation cannot work with a class that has multiple
// constructors (not counting copy/move constructors..?), and cannot construct a 
// class whose constructor has arguments with default values or a greater number
// of arguments than are accounted for in "generated_makeimpl.hpp".
//
// Assuming that the "automatically infer everything" approach is sugar to cover
// typical cases and not a hard requirement, we can certainly extend this to
// work for any class with at least one public constructor given a little bit of
// static configuration.

template<class Interface, class Impl>
class Injector
{
public:
    inline Injector(const Container& container, const init_params_placeholder_t<Interface>& params)
        : _container(container), _params(params)
    {
    }
    
    Interface* make() const
    {
        return make_impl<Impl>();
    }
    
private:
    template<std::size_t N>
    struct Arg
    {
        Arg(const Injector& injector_)
            : injector(injector_)
        {
        }
        
        template<typename ArgT,
            typename = typename std::enable_if<
                !std::is_same<_normalize_typename<ArgT>, Impl>::value
                && _is_lvalue_reference_arg<N, Interface, ArgT>::value
            >::type>
        inline operator ArgT&() const;
        
        template<typename ArgT,
            typename = typename std::enable_if<
                !std::is_same<_normalize_typename<ArgT>, Impl>::value
                && _is_rvalue_reference_arg<N, Interface, ArgT>::value
            >::type>
        inline operator ArgT&&() const;
        
        template<typename ArgT,
            typename = typename std::enable_if<
                !std::is_same<_normalize_typename<ArgT>, Impl>::value
                && !_is_lvalue_reference_arg<N, Interface, ArgT>::value
                && !_is_rvalue_reference_arg<N, Interface, ArgT>::value
            >::type>
        inline operator ArgT() const;
        
        const Injector& injector;
    };
    
    // Contains a set of generated definitions of `Impl_* make_impl<Impl_>()`
    // each corresponding to a number of arguments N between 0 and M inclusive
    // (M being 16 by default). The respective `Impl_* make_impl<Impl_>()`:
    //   1) is defined if and only if Impl_ has a constructor accepting N
    //      arguments.
    //   2) Calls that constructor, providing each of its arguments with an
    //      appropriate `Arg` object.
    //
    // This may not be the most conventionally elegant solution in C++1y, but it
    // is nevertheless effective and gives the compiler enough to work with that
    // it can infer everything it needs from the constructor arguments alone.
    //
    // "(addle root)/etc/tools/genmakeimpl.py" is used to generate this file.
    // THIS IS THE ONLY PLACE WHERE IT SHOULD BE INCLUDED.
#include "generated_makeimpl.hpp"
    
    const Container& _container;
    const init_params_placeholder_t<Interface>& _params;
};

class BindingSet
{
public:
    template<class Interface>
    class Binding
    {
    public:
        Binding() = default;
        Binding(const Binding&) = default;
        
        inline bool isSimple() const { return _filter.isSimple(); }
        inline bool isNull() const { return !_owner; }
        inline const BindingFilter& filter() const { return _filter; }
        
        inline Interface* make(const init_params_placeholder_t<Interface>& params) const
        {
            // assert not null
            return (this->*_make)(params);
        }
        
    private:
        typedef Interface* (Binding::* make_impl_t)(const init_params_placeholder_t<Interface>&) const;
        
        inline Binding(
                make_impl_t make_,
                BindingSet* owner,           
                const BindingFilter& filter
            )
            : _make(make_), _owner(owner), _filter(filter)
        {
        }
        
        template<class Impl>
        Interface* make_impl(const init_params_placeholder_t<Interface>& params) const
        {
            // assert _owner and _owner->_container
            return Injector<Interface, Impl>(*_owner->_container, params).make();      
        }
        
        template<class Impl>
        static inline QSharedPointer<Binding> bind_impl(BindingSet* owner, const BindingFilter& filter)
        {
            return QSharedPointer<Binding>(
                new Binding(&Binding::template make_impl<Impl>, owner, filter)
            );
        }
        
        make_impl_t _make;
        BindingFilter _filter;
        
        BindingSet* _owner = nullptr;
        
        friend class BindingSet;
        friend class BindingAdapter;
        friend class Container;
    };
    
    BindingSet() = default;
    BindingSet(const BindingSet&) = delete;
    
    template<class Interface, class Impl>
    void bind(BindingFilter filter = BindingFilter())
    {
        _bindings.insert(
            std::type_index(typeid(Interface)),
            Binding<Interface>::template bind_impl<Impl>(this, filter)
        );
    }
        
    template<class Interface>
    inline bool contains() const
    {
        return _bindings.contains(std::type_index(typeid(Interface)));
    }
    
private:
    const Container* _container = nullptr;
    QMultiHash<std::type_index, boost::any> _bindings;
    
    friend class Container;
};

class Container
{
public:
    
    template<class Interface>
    Interface& get() const;
    
    template<class Interface>
    QSharedPointer<BindingSet::Binding<Interface>> findBinding(
            const BindingSelector& selector = BindingSelector::from<Interface>()
    ) const;
    
    void addBindings(BindingSet* bindings)
    {
        bindings->_container = this;
        _bindings.push_front(std::unique_ptr<BindingSet>(bindings));
    }
    
private:
    template<class Interface>
    inline typename std::enable_if<
        has_static_binding<Interface>::value
    >::type pushStaticBinding() const;
    
    template<class Interface>
    inline typename std::enable_if<
        !has_static_binding<Interface>::value
    >::type pushStaticBinding() const;
    
    std::list<std::unique_ptr<BindingSet>> _bindings;
    
    // TODO: mutexes
    mutable BindingSet _staticBindings;
    mutable QHash<std::type_index, boost::any> _instances;
    mutable QHash<BindingSelector, boost::any> _simpleBindings;
};

template<std::size_t N, class Interface, typename ArgT>
struct ArgLookup
{
    ArgLookup(
        const init_params_placeholder_t<Interface>& params_,
        const Container& container_
    )
        : params(params_),
        container(container_)
    {
    }
  
    const init_params_placeholder_t<Interface>& params;
    const Container& container;
    
    ArgT value() const { return value_impl<ArgT>(); }
    
private:
    template<typename ArgT_>
    typename std::enable_if<
        _is_init_param<N, Interface>::value,
//         _is_convertible_init_param<N, Interface, ArgT_>::value,
        ArgT_
    >::type value_impl() const
    {
        return std::get<N>(params);
    }
    
//     template<typename ArgT_>
//     typename std::enable_if<
//         _is_init_param<N, Interface>::value
//         && !_is_convertible_init_param<N, Interface, ArgT_>::value,
//         ArgT_
//     >::type value_impl() const
//     {
//         return _parameter_converter<ArgT_, _init_param_element_t<N, Interface>>()(std::get<N>(params));
//     }
    
    template<typename ArgT_>
    typename std::enable_if<
        !_is_init_param<N, Interface>::value,
        ArgT_
    >::type value_impl() const
    {
        static_assert(
            Traits::is_singleton_gettable<_normalize_typename<ArgT_>>::value,
            "The requested interface is not gettable as a singleton."
        );
        static_assert(
            std::is_reference<ArgT_>::value,
            "Services must be requested by reference."
        );
        return container.get<_normalize_typename<ArgT_>>();
    }
};

template<std::size_t N, class Interface, typename ArgT>
struct ArgLookup<N, Interface, QSharedPointer<ArgT>>
{
    ArgLookup(
        const init_params_placeholder_t<Interface>& params_,
        const Container& container_
    )
        : params(params_),
        container(container_)
    {
    }
  
    const init_params_placeholder_t<Interface>& params;
    const Container& container;
    
    QSharedPointer<ArgT> value() const { return value_impl<ArgT>(); }
    
private:
    template<typename ArgT_>
    typename std::enable_if<
        _is_init_param<N, Interface>::value
        && std::is_convertible<
            _init_param_element_t<N, Interface>,
            QSharedPointer<ArgT_>
        >::value,
        QSharedPointer<ArgT_>
    >::type value_impl() const
    {
        return std::get<N>(params);
    }
    
    template<typename ArgT_>
    typename std::enable_if<
        _is_init_param<N, Interface>::value
        && !std::is_convertible<
            _init_param_element_t<N, Interface>,
            QSharedPointer<ArgT_>
        >::value,
        QSharedPointer<ArgT_>
    >::type value_impl() const
    {
        return _parameter_converter<
            QSharedPointer<ArgT_>,
            _init_param_element_t<N, Interface>
        >()(std::get<N>(params));
    }
    
    template<typename ArgT_>
    typename std::enable_if<
        !_is_init_param<N, Interface>::value,
        QSharedPointer<ArgT_>
    >::type value_impl() const
    {
        static_assert(
            Traits::is_public_makeable<_normalize_typename<ArgT_>>::value,
            "You may only request a new instance of a publicly makeable "
            "interface."
        );
        static_assert(
            !has_init_params<_normalize_typename<ArgT_>>::value,
            "You may not request a new instance of an interface that requires "
            "initialization parameters."
        );
        return container.findBinding<_normalize_typename<ArgT_>>()->make();
    }
};

template<std::size_t N, class Interface, typename ArgT>
struct ArgLookup<N, Interface, std::unique_ptr<ArgT>>
{
    ArgLookup(
        const init_params_placeholder_t<Interface>& params_,
        const Container& container_
    )
        : container(container_)
    {
        Q_UNUSED(params_);
    }

    const Container& container;
    
    std::unique_ptr<ArgT> value() const
    { 
        static_assert(
            !_is_init_param<N, Interface>::value,
            "A new interface instance was requested when an initialization "
            "parameter was expected."
        );
        static_assert(
            Traits::is_public_makeable<_normalize_typename<ArgT>>::value,
            "You may only request a new instance of a publicly makeable "
            "interface."
        );
        static_assert(
            !has_init_params<_normalize_typename<ArgT>>::value,
            "You may not request a new instance of an interface that requires "
            "initialization parameters."
        );
        return std::unique_ptr<ArgT>(
            container.findBinding<_normalize_typename<ArgT>>()->make(_nil_params())
        );
    }
};

template<class Interface, class Impl>
template<std::size_t N>
template<typename ArgT, typename>
inline Injector<Interface, Impl>::Arg<N>::operator ArgT&() const
{
    return ArgLookup<N, Interface, ArgT&>(
        injector._params,
        injector._container
    ).value();
}

template<class Interface, class Impl>
template<std::size_t N>
template<typename ArgT, typename>
inline Injector<Interface, Impl>::Arg<N>::operator ArgT&&() const
{
    return ArgLookup<N, Interface, ArgT&&>(
        injector._params,
        injector._container
    ).value();
}

template<class Interface, class Impl>
template<std::size_t N>
template<typename ArgT, typename>
inline Injector<Interface, Impl>::Arg<N>::operator ArgT() const
{
    return ArgLookup<N, Interface, ArgT>(
        injector._params,
        injector._container
    ).value();
}

template<class Interface>
Interface& Container::get() const
{
    static_assert(
        !has_init_params<Interface>::value,
        "Container::get() may not be used for interfaces that require init "
        "params. Those may only be instantiated by IFactory."
    );
    
    std::type_index index(typeid(_normalize_typename<Interface>));
    if (Q_LIKELY(_instances.contains(index)))
    {
        return *boost::any_cast<
            const QSharedPointer<_normalize_typename<Interface>>&
        >(
            _instances.value(index)
        );
    }
    
    auto binding = findBinding<Interface>(BindingSelector::from<Interface>());
    Interface* instance = binding->make(_nil_params());

    _instances.insert(index, QSharedPointer<Interface>(instance));
    return *instance;
}

template<class Interface>
QSharedPointer<BindingSet::Binding<Interface>> Container::findBinding(const BindingSelector& selector) const
{
    if (_simpleBindings.contains(selector))
    {
        return boost::any_cast<QSharedPointer<BindingSet::Binding<Interface>>>(
            _simpleBindings.value(selector)
        );
    }
    
    pushStaticBinding<Interface>();
    
//     for (const BindingSet& bindingSet : 
//             cpplinq::from(_bindings)
//             >>  cpplinq::deref()
//             >>  cpplinq::concat(
//                     cpplinq::singleton(_staticBindings)
//                 )
//         )
//     {
//         const auto& inner = bindingSet._bindings;
//         std::type_index index(selector.interface());
//         
//         auto&& end = inner.end();
//         auto i = inner.find(index);
//         while (i != end && i.key() == index)
//         {
//             auto binding = boost::any_cast<QSharedPointer<BindingSet::Binding<Interface>>>(*i);
//             
//             if (binding->filter().test(selector))
//             {
//                 if (binding->isSimple() && Traits::is_public_makeable<Interface>::value)
//                     _simpleBindings[selector] = binding;
//                 
//                 return binding;
//             }
//             ++i;
//         }
//     }
    
    ADDLE_THROW(
        BindingException(BindingException::BindingNotFound)
    );
}

template<class Interface>
inline typename std::enable_if<
    !has_static_binding<Interface>::value
>::type Container::pushStaticBinding() const
{
}

template<class Interface>
inline typename std::enable_if<
    has_static_binding<Interface>::value
>::type Container::pushStaticBinding() const
{
    if (!_staticBindings.contains<Interface>())
    {
        _staticBindings.bind<Interface, typename static_binding<Interface>::Impl>();
    }
}

} // namespace detail

}} // namespace Addle::Config

#endif // INJECTOR_HPP
