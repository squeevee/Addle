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

namespace Addle::config_detail {

template<typename Interface, typename Injector>
class RepositoryHelper
{
    using IdType = typename IRepository<Interface>::IdType;
    using data_t = QHash<IdType, LazyValue<QSharedPointer<Interface>>>;
    
public:
    RepositoryHelper(const Injector& injector_)
        : injector(const_cast<Injector&>(injector_))
    {
    }
    
    void add(IdType id)
    {
        if (data.contains(id))
            return;
        
        data[id] = [&, id] () {
            auto injector = boost::di::make_injector(
                boost::di::extension::make_extensible(injector),
                boost::di::bind<IdType>().to(id)[boost::di::override]
            );
            
            return QSharedPointer<Interface>(
                injector.template create<Interface*>()
            );
        };
    }
    
    void remove(IdType id)
    {
        if (!data.contains(id))
            return; // error?
            
        data.remove(id);
    }
    
    Injector& injector;
    data_t data;
};

template<typename Interface, typename Injector>
class SingletonRepository : public IRepository<Interface>
{
    using IdType = typename IRepository<Interface>::IdType;
    using data_t = typename IRepository<Interface>::data_t;
    
    static_assert(
        Traits::is_global_repo_gettable<Interface>::value,
        ""
    );
public:
    SingletonRepository(const Injector& injector)
        : _repo_helper(injector)
    {
    }
    
protected:
    const data_t& data_p() const override
    { 
        lazy_initialize();
        return _repo_helper.data;
    }
    
private:
    inline void lazy_initialize()
    {
        if (_isInitialized)
            return;
        
        // add all existing IDs of type and subscribe for future additions.
    }
    
    mutable bool _isInitialized = false;
    mutable RepositoryHelper<Interface, Injector> _repo_helper;
};

template<typename Interface, typename Injector>
class UniqueRepository : public IRepository<Interface>
{
    using IdType = typename IRepository<Interface>::IdType;
    using data_t = typename IRepository<Interface>::data_t;
    
    static_assert(
        Traits::is_local_repo_gettable<Interface>::value,
        ""
    );
public:
    UniqueRepository(const Injector& injector)
        : _repo_helper(injector)
    {
    }
    
protected:
    const data_t& data_p() const override { return _repo_helper.data; }
    
private:
    RepositoryHelper<Interface, Injector> _repo_helper;
};

} // namespace namespace Addle::config_detail
