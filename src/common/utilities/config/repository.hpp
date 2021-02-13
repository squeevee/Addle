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

// TODO: consolidate and simplify some of the feature permutations.

namespace Addle::config_detail {

template<typename Interface, typename Derived>
class RepositoryBaseCanTriviallyAddById
    : public virtual IRepositoryBaseCanTriviallyAddById<Interface>
{
    using IdType = typename repo_info<Interface>::IdType;
public:
    virtual ~RepositoryBaseCanTriviallyAddById() = default;
    
protected:
    void add_byIds(QList<IdType> ids) override;
    
private:
    Interface* make(IdType id) const;
};

template<typename Interface, typename Derived>
class RepositoryBaseCanAddByFactoryParams 
    : public virtual IRepositoryBaseCanAddByFactoryParams<Interface>
{
    using IdType = typename repo_info<Interface>::IdType;
public:
    virtual ~RepositoryBaseCanAddByFactoryParams() = default;
    
protected:
    
    class AddHandle : public IRepositoryBaseCanAddByFactoryParams<Interface>::IAddHandle
    {
    public:
        AddHandle(RepositoryBaseCanAddByFactoryParams<Interface, Derived>& repo)
            : _repo(repo)
        {
        }
        virtual ~AddHandle() = default;
        
        void add_by_params(const factory_params_t<Interface>&) override;
        
        QList<QSharedPointer<Interface>> submit() override;
        
    private:
        RepositoryBaseCanAddByFactoryParams<Interface, Derived>& _repo;
        QList<QSharedPointer<Interface>> _instances;
    };
        
    std::function<QList<QSharedPointer<Interface>>(QList<IdType>)>& boundAddFunction_p() override
    { return _boundAddFunction; }
    
    std::unique_ptr<
        typename IRepositoryBaseCanAddByFactoryParams<Interface>::IAddHandle
    > getAddHandle_p() override
    { return std::make_unique<AddHandle>(*this); }
    
private:
    std::function<QList<QSharedPointer<Interface>>(QList<IdType>)> _boundAddFunction;
};

template<typename Interface, typename Derived>
using repository_base = mp_apply_undeferred<
        mp_polymorphic_inherit,
        mp_build_list<
            boost::mp11::mp_bool<repo_info<Interface>::can_populate_by_id>,
                boost::mp11::mp_defer<RepositoryBaseCanTriviallyAddById,
                    Interface,
                    Derived
                >,
            boost::mp11::mp_valid<factory_params_t, Interface>,
                boost::mp11::mp_defer<RepositoryBaseCanAddByFactoryParams,
                    Interface,
                    Derived
                >
        >
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
                typename config_detail::factory_params_t<Interface>::value_types,
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
                    typename config_detail::factory_params_t<Interface>::value_types
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
    
    template<typename, typename> friend class RepositoryBaseCanTriviallyAddById;
    template<typename, typename> friend class RepositoryBaseCanAddByFactoryParams;
};

template<typename Interface, typename Derived>
void RepositoryBaseCanTriviallyAddById<Interface, Derived>::add_byIds(QList<IdType> ids)
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
Interface* RepositoryBaseCanTriviallyAddById<Interface, Derived>::make(IdType id) const
{
    if constexpr (has_factory_params<Interface>::value)
    {
        return std::apply(
            [this] (auto&&... args) {
                return static_cast<const Derived*>(this)->_factory.make(
                    std::forward<decltype(args)>(args)...
                );
            },
            generate_tuple_over_list<factory_params_t<Interface>>(
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
void RepositoryBaseCanAddByFactoryParams<Interface, Derived>
    ::AddHandle::add_by_params(const factory_params_t<Interface>& params) 
{
    Interface* instance = static_cast<Derived&>(_repo)._factory.make(params);
    _instances << QSharedPointer<Interface>(instance);
}

template<typename Interface, typename Derived>
QList<QSharedPointer<Interface>> RepositoryBaseCanAddByFactoryParams<Interface, Derived>
    ::AddHandle::submit()
{   
    for (auto instance : _instances)
    {
        LazyValue<QSharedPointer<Interface>> v;
        v.initialize(instance);
        
        static_cast<Derived&>(_repo)._data[instance->id()] = v;
    }
    
    return _instances;
}

} // namespace namespace Addle::config_detail
