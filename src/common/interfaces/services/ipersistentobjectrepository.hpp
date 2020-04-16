#ifndef IOBJECTREPOSITORY_HPP
#define IOBJECTREPOSITORY_HPP

#include <QSharedPointer>
#include <QHash>

#include "idtypes/persistentid.hpp"
#include "interfaces/traits/compat.hpp"

#include <type_traits>
#include "interfaces/traits/qobject_trait.hpp"

/**
 * A base interface for services that hold a collection of persisent objects
 * identified by a PersistentId subtype. Implementations of 
 * IPersistentObjectRepository specializations are expected to be experts on
 * the object type they represent
 */
template <class IdType, class ObjectType>
class IPersistentObjectRepository
{
    // static_assert(
    //     std::is_base_of<IdType, PersistentId>::value,
    //     "IdType must derive from PersistentId"
    // );

public:
    virtual ~IPersistentObjectRepository() = default;

    virtual QHash<IdType, QSharedPointer<ObjectType>> getCollection() = 0;
    virtual void setCollection(const QHash<IdType, QSharedPointer<ObjectType>>& collection) = 0;

    virtual QSharedPointer<ObjectType> get(const IdType& id) = 0;

    // slots:

    virtual void add(const IdType& id, QSharedPointer<ObjectType> object) = 0;
    virtual void add(const IdType& id, ObjectType* object) = 0;
    virtual void remove(const IdType& id) = 0; //hang on, should we even have this?

    // signals:

    virtual void collectionChanged(QList<IdType> ids) = 0;
};

#endif // IOBJECTREPOSITORY_HPP