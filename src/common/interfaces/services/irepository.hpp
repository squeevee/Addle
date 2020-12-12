#ifndef IREPOSITORY_HPP
#define IREPOSITORY_HPP

#include <typeinfo>

#include <QHash>
#include <QList>
#include <QSharedPointer>

#include "utilities/lazyvalue.hpp"

#include "interfaces/iamqobject.hpp"
#include "interfaces/traits.hpp"
#include "idtypes/addleid.hpp"

namespace Addle {

template<class Interface>
class IRepository
{
    static_assert(
        Traits::is_repo_gettable<Interface>::value,
        "A repository may only be instantiated for interfaces marked with "
        "DECL_GLOBAL_REPO_MEMBER or DECL_LOCAL_REPO_MEMBER."
    );
public:
    typedef typename Traits::repo_id_type<Interface>::type IdType;
protected:
    typedef QHash<IdType, LazyValue<QSharedPointer<Interface>>> data_t;
public:
    class Iterator
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
    };
        
    virtual ~IRepository() = default;
    
    inline Interface& get(IdType id) const { return *data_p().value(id); }
    inline Interface& operator[](IdType id) const { return *data_p().value(id); }
    
    inline Iterator begin() const { return Iterator(data_p().begin()); }
    inline Iterator end() const { return Iterator(data_p().end()); }
    
    inline QList<IdType> ids() const { return data_p().keys(); }
    
    virtual void add(QList<IdType> ids) = 0;
    inline void add(IdType id) { add(QList<IdType>({id})); }
    
    virtual void remove(QList<IdType> ids) = 0;
    inline void remove(IdType id) { remove(QList<IdType>({id})); }
    
protected:
    virtual const data_t& data_p() const = 0;
};

template<typename T>
class IFactory;

template<typename U>
class IFactory<IRepository<U>>
{
    static_assert(
        std::is_void<U>::value,
        "IFactory of IRepository is not allowed."
    );
};

namespace Traits {
    
template<typename Interface>
struct is_singleton_gettable<IRepository<Interface>> : std::true_type {};

template<typename Interface>
struct is_public_makeable<IRepository<Interface>> : std::true_type {};

} // namespace Traits
} // namespace Addle

#endif // IREPOSITORY_HPP
