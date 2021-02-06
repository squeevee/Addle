#pragma once 

#include <list>
#include <tuple>
#include <utility>

#include <boost/mp11.hpp>

#include <boost/type_traits/is_detected_convertible.hpp>
#include <boost/type_traits/is_complete.hpp>

#include <QMultiHash>

#include "interfaces/views/iviewbase.hpp"

#include "interfaces/traits.hpp"

#include "utilities/collections.hpp"
#include "utilities/lazyvalue.hpp"
#include "utilities/qobject.hpp"
#include "utilities/metaprogramming.hpp"

namespace Addle {
    
template<class T> class IFactory;
template<class View> class IViewRepository;

namespace aux_IViewRepository {
    
template<class View>
class iviewrepository_base_with_makeable_presenter
{
    using presenter_t = aux_view::presenter_of_t<View>;
    static_assert(Traits::is_makeable<presenter_t>::value);
    
public:
    virtual ~iviewrepository_base_with_makeable_presenter() = default;
    
    virtual const IFactory<presenter_t>& presenterFactory() const = 0;
    
    friend class IViewRepository<View>;
};

template<class View>
using _presenter_is_only_factory_parameter = boost::mp11::mp_and<
        boost::mp11::mp_bool<
            boost::mp11::mp_size<
                config_detail::factory_params_t<View>
            >::value == 1
        >,
        std::is_same<
            boost::mp11::mp_front<typename config_detail::factory_params_t<View>::tag_types>,
            aux_view::factory_param_tags::presenter
        >
    >;

template<class View>
using _can_populate_by_presenter = boost::mp11::mp_eval_if_not<
        config_detail::has_factory_params<View>,
        boost::mp11::mp_true,  // <- result if no factory params
        _presenter_is_only_factory_parameter,
        View
    >;

template<class View>
class iviewrepository_base_can_populate_by_presenter
{
    using presenter_t = aux_view::presenter_of_t<View>;
public:
    virtual ~iviewrepository_base_can_populate_by_presenter() = default;
    
protected:
    virtual void addByPresenter_p(presenter_t& presenter) = 0;
    
    friend class IViewRepository<View>;
};

template<typename View>
using iviewrepository_base = mp_apply_undeferred<
        mp_virtual_inherit,
        mp_build_list<
            Traits::is_makeable<aux_view::presenter_of_t<View>>,
                boost::mp11::mp_defer<iviewrepository_base_with_makeable_presenter, View>,
            _can_populate_by_presenter<View>,
                boost::mp11::mp_defer<iviewrepository_base_can_populate_by_presenter, View>
        >
    >;
    
template<class View>
using presenter_property_t = decltype(std::declval<const View>().presenter());
    
} // namespace aux_IViewRepository 
    
/**
 * A utility service that tracks a collection of views associated to presenters
 * and assists in managing their creation and lifetimes.
 * 
 * Features:
 * - Keeps an iterable bi-directional map of views and presenters.
 * - Automatically deletes views when their presenters are destroyed.
 * - Automatically removes references to views / presenters when they are
 *   destroyed elsewhere.
 * - Provides convenience accessors to view / presenter factories.
 * 
 * Tracking views by interface represents a particular challenge in C++, since
 * the objects themselves are owned and managed by an implementation-specific UI 
 * framework and few guarantees can be made in general about their lifetimes.
 * 
 * This service observes `QObject::destroyed` signals to maintain reference
 * safety and assumes all of the presenters and views have affinity with the 
 * main thread. References to views are only guaranteed safe if accessed from 
 * the main thread, and should not be kept between iterations of the main event
 * loop.
 * 
 * A possible future extension could expose instances by QPointer rather than
 * reference to permit a degree of safe persistent/asynchronous access.
 */
template<class View>
class IViewRepository : public aux_IViewRepository::iviewrepository_base<View>
{
public:
    using view_t = View;
    using presenter_t = aux_view::presenter_of_t<view_t>;
    
    static_assert(boost::is_complete<view_t>::value);
    static_assert(aux_view::is_view<view_t>::value);
    static_assert(Traits::implemented_as_QObject<view_t>::value);
    static_assert(Traits::is_makeable<view_t>::value);
    
    static_assert(boost::is_complete<presenter_t>::value);
    static_assert(Traits::implemented_as_QObject<presenter_t>::value);
    
protected:
    using data_t = QHash< presenter_t*, LazyValue<view_t*> >;
    
public:
    virtual ~IViewRepository() = default;
    
    virtual const IFactory<view_t>& factory() const = 0;
    
    inline view_t& get(const presenter_t& presenter) const
    {
        //TODO: assert main thread?
        return *(data_p()[const_cast<presenter_t*>(std::addressof(presenter))].value());
    }
    
    inline presenter_t& getPresenter(const view_t& view) const
    {
        //TODO: assert main thread?
        return this->getPresenter_impl<view_t>(view);
    }
    
    virtual void add(presenter_t& presenter, view_t& view) = 0;
    
    inline void add(presenter_t& presenter)
    {
        this->addByPresenter_impl<View>(presenter);
    }
    
    inline void add(QWeakPointer<presenter_t> presenter)
    {
        auto s_presenter = presenter.toStrongRef();
        
        assert(s_presenter); // exception or noop?
        
        this->addByPresenter_impl<View>(
            *s_presenter
        );
    }
    
protected:
    virtual const data_t& data_p() const = 0;
    
private:    
    template<
        typename View_, 
        typename std::enable_if_t<
            boost::is_detected_convertible<
                presenter_t&,
                aux_IViewRepository::presenter_property_t,
                View_
            >::value,
            void*
        > = nullptr
    >
    inline presenter_t& getPresenter_impl(const View_& view) const
    {
        return view.presenter();
    }
    
    // TODO: linear search if presenter property is not available ?
    
    template<
        typename View_, 
        typename std::enable_if_t<
            aux_IViewRepository::_can_populate_by_presenter<View_>::value,
            void*
        > = nullptr
    >
    inline void addByPresenter_impl(presenter_t& presenter)
    {
        static_cast<
            aux_IViewRepository::iviewrepository_base_can_populate_by_presenter<View_>*
        >(this)->addByPresenter_p(presenter);
    }
};

namespace Traits
{
    template<typename View> 
    struct is_singleton<IViewRepository<View>> : std::true_type {};
}

}
