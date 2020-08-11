#ifndef PERSISTENTOBJECTREPOSITORYHELPER_HPP
#define PERSISTENTOBJECTREPOSITORYHELPER_HPP

// OBSOLETE? 

#include "interfaces/services/ipersistentobjectrepository.hpp"
namespace Addle {
template<class IdType, class ObjectType>
class PersistentObjectRepositoryHelper
{
public: 
    PersistentObjectRepositoryHelper(IPersistentObjectRepository<IdType, ObjectType>& owner)
        : _owner(owner)
    {
    }

    QHash<IdType, QSharedPointer<ObjectType>> getCollection() { return _collection; }

    void setCollection(const QHash<IdType, QSharedPointer<ObjectType>>& collection)
    {
        _collection = collection;
        emit _owner.collectionChanged(_collection.keys());
    }

    QSharedPointer<ObjectType> get(const IdType& id)
    {
        //assert has
        return _collection.value(id);
    }

    void add(const IdType& id, QSharedPointer<ObjectType> object)
    {
        _collection.insert(id, object);
        emit _owner.collectionChanged(_collection.keys());
    }

    void add(const IdType& id, ObjectType* object)
    {
        _collection.insert(id, QSharedPointer<ObjectType>(object));
        emit _owner.collectionChanged(_collection.keys());
    }

    void remove(const IdType& id)
    {
        _collection.remove(id);
        emit _owner.collectionChanged(_collection.keys());
    }

private:
    QHash<IdType, QSharedPointer<ObjectType>> _collection;

    IPersistentObjectRepository<IdType, ObjectType>& _owner;
};

} // namespace Addle
#endif // PERSISTENTOBJECTREPOSITORYHELPER_HPP