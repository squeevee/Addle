/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#pragma once

#include "interfaces/traits.hpp"

#include <boost/di/extension/injections/extensible_injector.hpp>
#include "interfaces/services/irepository.hpp"

#include "injectbundle.hpp"

namespace Addle::config_detail {

template<typename Interface, typename Derived>
class repository_base_add_by_id 
    : public virtual irepository_base_add_by_id<Interface>
{
    using IdType = typename repo_info<Interface>::IdType;
public:
    virtual ~repository_base_add_by_id() = default;
    
protected:
    void add_byId(QList<IdType> ids) override;
    
private:
    Interface* make(IdType id) const;
};

template<class Interface, typename Derived>
using _deferred_repository_base_add_by_id = mp_optional_eval_t<
        boost::mp11::mp_valid<_deferred_irepository_base_add_by_id, Interface>,
        repository_base_add_by_id,
        Interface,
        Derived
    >;

template<typename Interface, typename Derived>
class repository_base_add_by_factory_params 
    : public virtual irepository_base_add_by_factory_params<Interface>
{
    using IdType = typename repo_info<Interface>::IdType;
public:
    virtual ~repository_base_add_by_factory_params() = default;
    
protected:
    Interface& add_new_by_params_p(const factory_params_t<Interface>&) override;
};

template<class Interface, typename Derived>
using _deferred_repository_base_add_by_factory_params = mp_optional_eval_t<
        boost::mp11::mp_valid<_deferred_irepository_base_add_by_factory_params, Interface>,
        repository_base_add_by_factory_params,
        Interface,
        Derived
    >;
    
template<typename Interface, typename Derived>
using repository_base = boost::mp11::mp_apply<
        mp_polymorphic_inherit,
        typename mp_where_valid<
            _deferred_repository_base_add_by_id,
            _deferred_repository_base_add_by_factory_params
        >::template fn<Interface, Derived>
    >;

template<typename Interface>
class Repository 
    :   public repository_base<Interface, Repository<Interface>>, 
        public IRepository<Interface>
{
    using typename IRepository<Interface>::IdType;
    using typename IRepository<Interface>::data_t;
    
    static_assert(
        boost::mp11::mp_all_of_q<
                typename config_detail::repo_info<Interface>
                        ::_interface_factory_parameters::value_types,
                boost::mp11::mp_compose<
                    boost::remove_cv_ref_t, 
                    boost::is_complete
                >
            >::value,
        "One or more of the factory parameters for this interface was an "
        "incomplete type."
    );
        
    // Resolves to an InjectBundle whose template parameters are
    // a `const IRepository<T>&` for every T needed to create Interface.
    using repo_deps_t = 
        boost::mp11::mp_apply<
            InjectBundle,
            boost::mp11::mp_transform_q<
                boost::mp11::mp_compose<
                    boost::remove_cv_ref_t, 
                    IRepository, 
                    std::add_const_t, 
                    std::add_lvalue_reference_t
                >,
                boost::mp11::mp_filter_q<
                    boost::mp11::mp_compose<
                        boost::remove_cv_ref_t, 
                        Traits::is_singleton_repo_member
                    >,
                    typename config_detail::repo_info<Interface>
                        ::_interface_factory_parameters::value_types
                >
            >
        >;
        
public:
    Repository(
            const IFactory<Interface>& factory, 
            repo_deps_t repoDependencies
        ) 
        : _factory(factory), 
        _repoDependencies(repoDependencies)
    {
    }
    
    virtual ~Repository() = default;
    
    
    void add(QList<QSharedPointer<Interface>> instances) override
    {
        for (auto instance : instances)
        {
            LazyValue<QSharedPointer<Interface>> v;
            v.initialize(QSharedPointer<Interface>(instance));
            _data.insert(instance->id(), v);
        }
    }
    
    void remove(QList<IdType> ids) override 
    {
        //TODO
    }
    
protected:
    const data_t& data_p() const override { return _data; }
    
private:
    const IFactory<Interface>& _factory;
    repo_deps_t _repoDependencies;
    data_t _data;
    
    template<typename, typename> friend class repository_base_add_by_id;
    template<typename, typename> friend class repository_base_add_by_factory_params;
};

template<typename Interface, typename Derived>
void repository_base_add_by_id<Interface, Derived>::add_byId(QList<IdType> ids)
{
    for (auto id : ids)
    {
        LazyValue<QSharedPointer<Interface>> v(
            [this, id] () -> QSharedPointer<Interface> {
                return QSharedPointer<Interface>(this->make(id));
            }
        );
        
        static_cast<Derived*>(this)->_data.insert(id, v);
    }
}

template<typename Interface, typename Derived>
Interface* repository_base_add_by_id<Interface, Derived>::make(IdType id) const
{
    if constexpr (has_factory_params<Interface>::value)
    {
        return std::apply(
            [this] (auto&&... args) {
                return static_cast<const Derived*>(this)->_factory.make(
                    std::forward<decltype(args)>(args)...
                );
            },
            generate_tuple_over<factory_params_t<Interface>>(
                [this, id] (auto i) {
                    using Tag = typename decltype(i)::type::tag_type;
                    using Value = typename decltype(i)::type::value_type;
                    if constexpr (
                            std::is_same<
                                Tag, 
                                generic_id_parameter::factory_param_tags::id
                            >::value
                        )
                    {
                        return generic_id_parameter::id_ = id;
                    }
                    else
                    {
                        const auto& otherRepo = static_cast<const Derived*>(this)
                            ->_repoDependencies.template value<
                                const IRepository<boost::remove_cv_ref_t<Value>>&
                            >();
                            
                        return boost::parameter
                            ::keyword<Tag>::instance = otherRepo.get(id);
                    }
                }
            )
        );
    }
    else // no factory parameters
    {
        return static_cast<const Derived*>(this)->_factory.make();
    }
}

template<typename Interface, typename Derived>
Interface& repository_base_add_by_factory_params<Interface, Derived>
    ::add_new_by_params_p(const factory_params_t<Interface>& params) 
{
    Interface* instance = static_cast<Derived*>(this)->_factory.make(params);
    
    LazyValue<QSharedPointer<Interface>> v;
    v.initialize(QSharedPointer<Interface>(instance));
    
    static_cast<Derived*>(this)->_data.insert(instance->id(), v);
    
    return *instance;
}

} // namespace namespace Addle::config_detail
