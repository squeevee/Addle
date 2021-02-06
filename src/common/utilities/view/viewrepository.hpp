#ifndef VIEWREPOSITORY_HPP
#define VIEWREPOSITORY_HPP

#include "interfaces/services/ifactory.hpp"
#include "interfaces/views/iviewrepository.hpp"

#include "utilities/config/injectbundle.hpp"

namespace Addle {
    
namespace aux_ViewRepository {
  
template<class View>
using variadic_deps_t = mp_apply_undeferred<
        config_detail::InjectBundle,
        mp_build_list<
            Traits::is_makeable<aux_view::presenter_of_t<View>>,
            boost::mp11::mp_defer<
                boost::mp11::mp_compose<
                    aux_view::presenter_of_t,
                    IFactory,
                    std::add_const_t,
                    std::add_lvalue_reference_t
                >::template fn, // const IFactory<aux_view::presenter_of_t<View>>&
                View
            >
        >
    >;
    
template<class View, class Derived>
class viewrepository_base_with_makeable_presenter 
    : public virtual aux_IViewRepository::iviewrepository_base_with_makeable_presenter<View>
{
    using presenter_t = aux_view::presenter_of_t<View>;
public:
    virtual ~viewrepository_base_with_makeable_presenter() = default;
    
    const IFactory<presenter_t>& presenterFactory() const override;
};
    
template<class View, class Derived>
class viewrepository_base_can_populate_by_presenter 
    : public virtual aux_IViewRepository::iviewrepository_base_can_populate_by_presenter<View>
{
    using presenter_t = aux_view::presenter_of_t<View>;
public:
    virtual ~viewrepository_base_can_populate_by_presenter() = default;
    
protected:
    void addByPresenter_p(presenter_t& presenter) override;
};

template<class View, class Derived>
using viewrepository_base = mp_apply_undeferred<
        mp_polymorphic_inherit,
        mp_build_list<
            Traits::is_makeable<aux_view::presenter_of_t<View>>,
                boost::mp11::mp_defer<viewrepository_base_with_makeable_presenter, View, Derived>,
            aux_IViewRepository::_can_populate_by_presenter<View>,
                boost::mp11::mp_defer<viewrepository_base_can_populate_by_presenter, View, Derived>
        >
    >;
    
//class viewrepository_base
    
} // namespace aux_ViewRepository 
    
template<class View>
class ViewRepository 
    : public IViewRepository<View>, 
    public aux_ViewRepository::viewrepository_base<View, ViewRepository<View>>
{
    using typename IViewRepository<View>::view_t;
    using typename IViewRepository<View>::presenter_t;
    
    using typename IViewRepository<View>::data_t;
    
    using variadic_deps_t = aux_ViewRepository::variadic_deps_t<View>;
public:
    inline ViewRepository(
            const IFactory<view_t>& factory,
            variadic_deps_t&& variadicDeps
        )
        : _factory(factory),
        _variadicDeps(std::move(variadicDeps))
    {
    }
    
    virtual ~ViewRepository() = default;
    
    void add(presenter_t& presenter, view_t& view) override
    {
        LazyValue<view_t*> v;
        v.initialize(std::addressof(view));
        
        _data.insert(std::addressof(presenter), v);
    }
    
    const IFactory<view_t>& factory() const override { return _factory; }
    
protected:
    const data_t& data_p() const override { return _data; }
    
private:
    const IFactory<view_t>& _factory;
    variadic_deps_t _variadicDeps;
    
    data_t _data;
    
    template<class, class> friend class aux_ViewRepository::viewrepository_base_with_makeable_presenter;
    template<class, class> friend class aux_ViewRepository::viewrepository_base_can_populate_by_presenter;
};
    
template<class View, class Derived>
const IFactory<aux_view::presenter_of_t<View>>& 
aux_ViewRepository::viewrepository_base_with_makeable_presenter<View, Derived>::presenterFactory() const
{
    return static_cast<const Derived*>(this)
        ->_variadicDeps.template value<const IFactory<presenter_t>&>();
}

template<class View, class Derived>
void aux_ViewRepository::viewrepository_base_can_populate_by_presenter<View, Derived>::addByPresenter_p(presenter_t& presenter)
{
    LazyValue<View*> v(
        [this, &presenter] () -> View* {
            return static_cast<Derived*>(this)->_factory.make(
                aux_view::presenter_ = presenter
            );
        }
    );
    
    static_cast<Derived*>(this)->_data.insert(std::addressof(presenter), v);
}

}

#endif //VIEWREPOSITORY_HPP
