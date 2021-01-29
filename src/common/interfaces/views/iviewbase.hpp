#pragma once

#include <QSharedPointer>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {
    
namespace aux_IViewBase {
    class reference_presenter_tag {};
    class shared_presenter_tag {};
    
    ADDLE_FACTORY_PARAMETER_NAME(presenter)
}

/**
 * Base interface (template) for views in Addle, associating every view to a
 * presenter interface given by PresenterType_
 * 
 * If PresenterHandle is aux_IViewBase::reference_presenter_tag, then the
 * presenter is handled by mutable reference. If PresenterHandle is 
 * aux_IViewBase::shared_presenter_tag then it is handled by QSharedPointer.
 */
template<
    class PresenterType_, 
    class PresenterHandle = aux_IViewBase::reference_presenter_tag
>
class IViewBase : public virtual IAmQObject
{
    static_assert(
        std::is_same<PresenterHandle, aux_IViewBase::reference_presenter_tag>::value,
        "Unexpected type given for PresenterHandle"
    );
public:
    using PresenterType = PresenterType_;
    virtual ~IViewBase() = default;
    
    virtual PresenterType& presenter() const = 0;
};

template<class PresenterType_>
class IViewBase<PresenterType_, aux_IViewBase::shared_presenter_tag> 
    : public virtual IAmQObject
{
public:
    using PresenterType = PresenterType_;
    virtual ~IViewBase() = default;
    
    virtual QSharedPointer<PresenterType> presenter() const = 0;
};

namespace aux_IViewBase::detail {
    template<typename T>
    using _presenter_type = typename T::PresenterType;
        
    template<typename T>
    using _presenter_handle = boost::mp11::mp_if<
            std::is_base_of<
                IViewBase<_presenter_type<T>, shared_presenter_tag>,
                T
            >,
            shared_presenter_tag,
            reference_presenter_tag
        >;
    
    template<typename T>
    using _presenter_param_impl = boost::mp11::mp_if<
            std::is_same<_presenter_handle<T>, shared_presenter_tag>,
            QSharedPointer<_presenter_type<T>>,
            _presenter_type<T>&
        >;
    
    // Gives the type expected by the "presenter" parameter of a given view.
    // For views deriving from IViewBase<..., shared_presenter_tag> this will
    // be QSharedPointer<T::PresenterType>, and for all other views, it will be
    // T::PresenterType&
    template<typename T>
    struct presenter_param
    {
        static_assert(
            boost::mp11::mp_valid<_presenter_type, T>::value,
            "This metafunction may only be used if T::PresenterType is defined."
        );
        using type = _presenter_param_impl<T>;
    };
} // aux_IViewBase::detail

} // namespace Addle

#define ADDLE_VIEW_FACTORY_PARAMETERS_basic(interface)                      \
    ADDLE_DECL_FACTORY_PARAMETERS(                                               \
        interface,                                                          \
        (required                                                           \
            ( presenter,                                                    \
                (typename ::Addle::aux_IViewBase::detail                    \
                    ::presenter_param<interface>::type)                     \
            )                                                               \
        )                                                                   \
    )

#define ADDLE_VIEW_FACTORY_PARAMETERS_extended(interface, signature)        \
    ADDLE_DECL_FACTORY_PARAMETERS(                                               \
        interface,                                                          \
        (required                                                           \
            ( presenter,                                                    \
                (typename ::Addle::aux_IViewBase::detail                    \
                    ::presenter_param<interface>::type)                     \
            )                                                               \
        )                                                                   \
        signature                                                           \
    )

/**
 * @macro ADDLE_VIEW_FACTORY_PARAMETERS
 * Usage: ADDLE_VIEW_FACTORY_PARAMETERS(interface[, signature])
 * 
 * Defines factory parameters for the view interface. One parameter is
 * automatically generated: "presenter", of the type expected by the view (see 
 * `aux_IViewBase::detail::presenter_param`).
 * 
 * @note The name object and tag type for the "presenter" parameter is in the
 * aux_IViewBase namespace, but this macro assumes the interface has its own
 * aux_*** namespace with its parameters there (as with factory parameters for
 * other interfaces). It is recommended to place 
 * `using namespace aux_IViewBase;` into the interface aux namespace to 
 * "inherit" the parameter by automatically creating the necessary aliases.
 * 
 * Additional factory params can be specified as the optional second argument of
 * this macro, with the same structure as ADDLE_DECL_FACTORY_PARAMETERS. These will
 * be placed *after* the "presenter" parameter.
 */
#define ADDLE_VIEW_FACTORY_PARAMETERS(...)                                  \
    BOOST_PP_IF(                                                            \
        BOOST_PP_EQUAL(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1),             \
        ADDLE_VIEW_FACTORY_PARAMETERS_basic,                                \
        ADDLE_VIEW_FACTORY_PARAMETERS_extended                              \
    )(__VA_ARGS__)
