#ifndef IREPOSITORY_HPP
#define IREPOSITORY_HPP

#include <typeinfo>
#include <boost/type_traits.hpp>
#include <boost/type_traits/is_detected_exact.hpp>

#include <QHash>
#include <QList>
#include <QSharedPointer>

#include "utilities/lazyvalue.hpp"

#include "interfaces/iamqobject.hpp"
#include "interfaces/traits.hpp"
#include "interfaces/services/ifactory.hpp"

#include "idtypes/addleid.hpp"

#include "utilities/metaprogramming.hpp"
#include "utilities/config/storedparameters.hpp"

namespace Addle {
    
template<typename Interface>
class IRepository;
    
namespace config_detail {

template<typename Interface>
struct repo_info
{
    using IdType = typename Traits::repo_id_type<Interface>::type;
    
    template<typename T>
    using _is_common_id_repo_member = boost::mp11::mp_and<
            Traits::is_singleton_repo_member<boost::remove_cv_ref_t<T>>,
            std::is_same<
                IdType,
                typename Traits::repo_id_type<boost::remove_cv_ref_t<T>>::type
            >
        >;
        
    template<typename T>
    using _type_can_populate_by_id = boost::mp11::mp_eval_or<
            std::is_same<IdType, T>,
            _is_common_id_repo_member,
            T
        >;
    
    static constexpr bool can_populate_by_id = boost::mp11::mp_all_of<
            typename factory_params_t<Interface>::value_types,
            _type_can_populate_by_id
        >::value;
        
    using data_t = QHash< IdType, LazyValue<QSharedPointer<Interface>> >;
};

template<class Interface>
class IRepositoryBaseCanTriviallyAddById
{
    using IdType = typename repo_info<Interface>::IdType;
public: 
    virtual ~IRepositoryBaseCanTriviallyAddById() = default;
    
protected:
    virtual void add_byIds(QList<IdType> ids) = 0;
    
    friend class IRepository<Interface>;
};

template<class Interface>
class IRepositoryBaseCanAddByFactoryParams
{
    using IdType = typename repo_info<Interface>::IdType;
protected:
    class IAddHandle
    {
    public:
        virtual ~IAddHandle() = default;
        
        virtual void add_by_params(const factory_params_t<Interface>&) = 0;
        virtual QList<QSharedPointer<Interface>> submit() = 0;
    };
    
    template<typename... Args>
    struct boundAddFunction_impl
    {
        boundAddFunction_impl(
                IRepositoryBaseCanAddByFactoryParams<Interface>& repo_, 
                Args&&... args
            )
            : repo(repo_), 
            params(std::forward<Args>(args)...)
        {
        }
        
        QList<QSharedPointer<Interface>> operator()(QList<IdType> ids) const
        {
            auto handle = repo.getAddHandle_p();
            auto dispatcher = make_factory_param_dispatcher<Interface>(
                    std::bind(
                        &IAddHandle::add_by_params,
                        handle.get(),
                        std::placeholders::_1
                    )
                );
            
            for (auto id : ids)
            {
                if constexpr (boost::mp11::mp_contains<
                        typename factory_params_t<Interface>::tag_types,
                        generic_id_parameter::factory_param_tags::id
                    >::value)
                {
                    std::apply(
                        dispatcher,
                        std::tuple_cat(
                            std::make_tuple( generic_id_parameter::id_ = id ),
                            params.toArgTuple()
                        )
                    );
                }
                else
                {
                    std::apply(
                        dispatcher,
                        params.toArgTuple()
                    );
                }
            }
            
            return handle->submit();
        }
        
        IRepositoryBaseCanAddByFactoryParams<Interface>& repo;
        stored_parameters<Args...> params;
    };
    
public:
    virtual ~IRepositoryBaseCanAddByFactoryParams() = default;
        
    template<typename... Args>
    inline Interface& addNew(Args&&... args)
    {
        auto handle = this->getAddHandle_p();
        auto dispatcher = make_factory_param_dispatcher<Interface>(
                std::bind(
                    &IAddHandle::add_by_params,
                    handle.get(),
                    std::placeholders::_1
                )
            );
        
        dispatcher(std::forward<Args>(args)...);
        auto instances = handle->submit();
        
        assert(instances.count() == 1);
        return *(instances[0]);
    }
    
    template<typename... Args>
    inline void bindFactoryParameters(Args&&... args)
    {
//         static_assert(
//             boost::mp11::mp_all_of<
//                 boost::mp11::mp_list<std::decay_t<Args>...>,
//                 std::is_copy_constructible
//             >::value,
//             "One or more arguments is not copy constructible. Binding these "
//             "arguments is not allowed."
//         );
        
        boundAddFunction_p() = 
            boundAddFunction_impl<Args...>(
                *this,
                std::forward<Args>(args)...
            );
    }
    
    inline void unbindFactoryParameters()
    {
        boundAddFunction_p() = {};
    }
    
protected:
    virtual std::function<QList<QSharedPointer<Interface>>(QList<IdType>)>& boundAddFunction_p() = 0;
    virtual std::unique_ptr<IAddHandle> getAddHandle_p() = 0;
    
    friend class IRepository<Interface>;
};

template<class Interface>
using irepository_base = mp_apply_undeferred<
        mp_virtual_inherit,
        mp_build_list<
            boost::mp11::mp_bool<repo_info<Interface>::can_populate_by_id>,
                boost::mp11::mp_defer<IRepositoryBaseCanTriviallyAddById, Interface>,
            config_detail::has_factory_params<Interface>, 
                boost::mp11::mp_defer<IRepositoryBaseCanAddByFactoryParams, Interface>
        >
    >;

template<typename Interface>
using _typeof_id_property = decltype( std::declval<const Interface>().id() );

template<typename Interface>
using has_id_property = boost::is_detected_exact<
        typename repo_info<Interface>::IdType,
        _typeof_id_property, 
        Interface
    >;

} // namespace config_detail

template<class Interface>
class IRepository : public config_detail::irepository_base<Interface>
{
    static_assert(
        boost::is_complete<Interface>::value,
        "A repository may only be instantiated for complete interfaces."
    );
    static_assert(
        Traits::is_repo_member<Interface>::value,
        "A repository may only be instantiated for interfaces marked with "
        "ADDLE_DECL_SINGLETON_REPO_MEMBER or ADDLE_DECL_UNIQUE_REPO_MEMBER."
    );
    static_assert(
        config_detail::has_id_property<Interface>::value,
        "An interface must expose a public const `id` property of type IdType "
        "to be used in a repository."
    );
public:
    using IdType = typename config_detail::repo_info<Interface>::IdType;
        
protected:
    using data_t = typename config_detail::repo_info<Interface>::data_t;
    
public:
    class Iterator
        // TODO: boost iterator adapter
    {
    public:
        Iterator() = default;
        Iterator(const Iterator&) = default;
        
        inline Iterator& operator++() { ++_inner; return *this; }
        inline Iterator operator++(int) { return Iterator(_inner++); }
        
        inline Iterator& operator--() { --_inner; return *this; }
        inline Iterator operator--(int) { return Iterator(_inner--); }
        
        inline Interface& operator*() const
        {
            return **_inner;
        }
        
        inline Interface* operator->() const
        {
            return (*_inner).data();
        }
        
    private:
        inline Iterator(typename data_t::const_iterator inner)
            :_inner(inner)
        {
        }
        typename data_t::const_iterator _inner;
        
        friend class IRepository;
    };
        
    virtual ~IRepository() = default;
    
    inline Interface& get(IdType id) const { return *(data_p()[id].value()); }
    inline Interface& operator[](IdType id) const { return get(id); }
    
    inline QSharedPointer<Interface> getShared(IdType id) const { return data_p()[id].value(); }
    
    inline Iterator begin() const { return Iterator(data_p().begin()); }
    inline Iterator end() const { return Iterator(data_p().end()); }
    
    inline QList<IdType> ids() const { return data_p().keys(); }
        // TODO: transform range (modestly cheaper, probably)
        
    virtual void add(QList<QSharedPointer<Interface>> instances) = 0;
    
    inline void add(std::initializer_list<QSharedPointer<Interface>> instances)
    {
        add(QList<QSharedPointer<Interface>>(instances));
    }
    
    inline void add(QSharedPointer<Interface> instance)
    {
        add(QList<QSharedPointer<Interface>>({instance}));
    }
    
    inline void add(QList<IdType> ids)
    {
        this->add_byIds_<Interface>(ids);
    }
        
    inline void add(std::initializer_list<IdType> ids)
    {
        this->add_byIds_<Interface>(QList<IdType>(ids));
    }
    
    inline void add(IdType id)
    { 
        this->add_byIds_<Interface>(QList<IdType>({id})); 
    }
    
    template<typename L>
    inline void addStaticIds()
    {
        constexpr auto ids = const_array_from_list<L, IdType>();
        this->add_byIds_<Interface>(QList<IdType>( ids.begin(), ids.end() ));
    }
        
    virtual void remove(QList<IdType> ids) = 0;
    inline void remove(IdType id) { remove(QList<IdType>({id})); }    
    
protected:
    virtual const data_t& data_p() const = 0;
    
private:
    template<
            typename Interface_, 
            std::enable_if_t<
                config_detail::repo_info<Interface_>::can_populate_by_id, 
                void*> = nullptr
        >
    inline void add_byIds_(QList<IdType> ids)
    {
        if constexpr (config_detail::has_factory_params<Interface>::value)
        {
            const auto& boundAddFunction = static_cast<config_detail
                ::IRepositoryBaseCanAddByFactoryParams<Interface>*>(this)
                    ->boundAddFunction_p();
                
            if (boundAddFunction)
            {
                boundAddFunction(ids);
                return;
            }
        }
        
        static_cast<config_detail::IRepositoryBaseCanTriviallyAddById<Interface>*>(this)
            ->add_byIds(ids);
    }
    
    template<
        typename Interface_, 
        std::enable_if_t<
            !config_detail::repo_info<Interface_>::can_populate_by_id
            && config_detail::has_factory_params<Interface>::value, 
            void*> = nullptr
    >
    inline void add_byIds_(QList<IdType> ids)
    {
        const auto& boundAddFunction = static_cast<config_detail
            ::IRepositoryBaseCanAddByFactoryParams<Interface>*>(this)
                ->boundAddFunction_p();
                
        assert(boundAddFunction); // TODO: exception?
        boundAddFunction(ids);
    }
};

template<typename T>
class IFactory;

template<typename U>
class IFactory<IRepository<U>>
{
    static_assert(std::is_void<U>::value, "IFactory of IRepository is not allowed.");
};

namespace Traits {
    
template<typename T>
struct is_repository : std::false_type {};

template<typename Interface>
struct is_repository<IRepository<Interface>> : std::true_type {};
    
template<typename Interface>
struct is_singleton<IRepository<Interface>> : std::true_type {};

template<typename Interface>
struct is_makeable<IRepository<Interface>> : std::true_type {};

} // namespace Traits
} // namespace Addle

#endif // IREPOSITORY_HPP
