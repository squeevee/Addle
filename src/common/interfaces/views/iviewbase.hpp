#pragma once

#include <QSharedPointer>

#include <utility>

#include <boost/mp11.hpp>
#include "utilities/config/factoryparams.hpp"

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {
    
template<class PresenterType_>
class IViewBase : public virtual IAmQObject
{
public:
    using PresenterType = PresenterType_;
    virtual ~IViewBase() = default;
    
    virtual PresenterType& presenter() const = 0;
};

namespace aux_view {
      
// Generic factory parameter for a view's presenter. Be sure to put an alias to
// this parameter in your interface's aux_*** namespace, as certain automated
// utilities expect it in order to work with views.
ADDLE_FACTORY_PARAMETER_NAME(presenter)

// Specialize this trait to explicitly define a presenter for a given view
template<typename View>
struct explicit_presenter_of {};

template<typename View>
using _explicit_presenter_of_t = typename explicit_presenter_of<View>::type;

template<typename View>
using _member_presenter_of_t = typename View::PresenterType;

/**
 * Gives the presenter of a view.
 * 
 * If explicit_presenter_of is a valid type for View, this is an alias for that
 * type. Otherwise, is an alias for View::PresenterType
 */
template<typename View>
using presenter_of_t = typename boost::mp11::mp_if<
        boost::mp11::mp_valid<_explicit_presenter_of_t, View>,
        boost::mp11::mp_defer<_explicit_presenter_of_t, View>,
        boost::mp11::mp_defer<_member_presenter_of_t, View>
    >::type;

// Alias for a true type if presenter_of_t is valid for T and T is derived from
// IViewBase<presenter_of_t<T>>. Alias for a false type otherwise.
template<typename T>
using _is_view_impl = boost::mp11::mp_eval_if_not_q<
        boost::mp11::mp_valid<presenter_of_t, T>,
        boost::mp11::mp_false,  // <- result if no presenter
        boost::mp11::mp_bind<
            std::is_base_of,
            boost::mp11::mp_bind_q<
                boost::mp11::mp_compose<presenter_of_t, IViewBase>,
                boost::mp11::_1
            >,
            boost::mp11::_1
        >,
        T
    >;
    
/**
 * This trait gives whether a type is a view interface. 
 * 
 * The default implementation is true if T has a presenter (as given by
 * `presenter_of_t`) and T is derived from IViewBase for that presenter type.
 * 
 * This trait can be specialized for any views not derived from IViewBase
 */
template<typename T>
struct is_view : _is_view_impl<T> {};

} // namespace aux_view

} // namespace Addle

#define ADDLE_VIEW_FACTORY_PARAMETERS_basic(interface)                      \
    ADDLE_DECL_FACTORY_PARAMETERS(                                          \
        interface,                                                          \
        (required                                                           \
            ( presenter,                                                    \
                (::Addle::aux_view::presenter_of_t<interface>&)             \
            )                                                               \
        )                                                                   \
    )

#define ADDLE_VIEW_FACTORY_PARAMETERS_extended(interface, signature)        \
    ADDLE_DECL_FACTORY_PARAMETERS(                                          \
        interface,                                                          \
        (required                                                           \
            ( presenter,                                                    \
                (::Addle::aux_view::presenter_of_t<interface>&)             \
            )                                                               \
        )                                                                   \
        signature                                                           \
    )

/**
 * @macro ADDLE_VIEW_FACTORY_PARAMETERS
 * Usage: ADDLE_VIEW_FACTORY_PARAMETERS(interface[, signature])
 * 
 * Defines factory parameters for the given view interface. One parameter is
 * automatically generated: "presenter", of the type expected by the view (see 
 * `aux_IViewBase::detail::presenter_param`).
 * 
 * @note The name object and tag type for the generic "presenter" parameter is
 * in the aux_view namespace
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
