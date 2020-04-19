#ifndef PERSISTENTID_HPP
#define PERSISTENTID_HPP

#include <QSharedData>
#include <QSharedDataPointer>
#include <QHash>
#include <QVariant>

#include "utilities/indexvariant.hpp"

/**
 * @class PersistentId
 * @brief Base class for identifier objects that are persistent and unique 
 * throughout runtime.
 * 
 * PersistentId objects identify various resources within Addle that persist
 * through runtime, but are not necessarily known at compile-time. Derived
 * classes of PersistentId denote more specific information about these
 * resources, including metadata.
 * 
 * Each PersistentId object has a QString key that should be unique, and can be
 * used for serialization. PersistentId is hashable and can convert to QVariant
 * and IndexVariant.
 * 
 * For convenience, arbitrary upcasts and downcasts are allowed between 
 * PersistentId and its derived classes. Each PersistentIdData instance keeps
 * track of the Qt meta-type id of its owner, so illegal upcasts result in a
 * null ID. Derived classes should have no members: use the metadata QHash to
 * store any associated values instead.
 * 
 * Be sure to use the `PERSISTENT_ID_BOILERPLATE` macros (there are two of them)
 * for derived classes, as well as declaring and registering meta-type, meta-
 * converters, and an IndexVariant hasher.
 */
class PersistentId
{
    struct PersistentIdData : QSharedData
    {
        PersistentIdData(int metaTypeId_arg, QString key_arg, QHash<QString, QVariant> metadata_arg)
            : metaTypeId(metaTypeId_arg), key(key_arg), metadata(metadata_arg)
        {
        }

        int metaTypeId;
        QString key;
        QHash<QString, QVariant> metadata;
    };
public:
    PersistentId() : _data(nullptr) { }
    PersistentId(const PersistentId& other)
        : _data(other._data)
    {
    }

    inline QString getKey() const { return _data->key; }

    inline bool isNull() const { return !_data; }
    explicit inline operator bool() const { return !isNull(); }
    inline bool operator!() const { return isNull(); }

    inline bool operator==(const PersistentId& other) const
    {
        return _data == other._data;
    }
    inline bool operator!=(const PersistentId& other) const
    {
        return _data != other._data;
    }

    inline operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
    inline operator IndexVariant() const
    {
        return IndexVariant(QVariant::fromValue(*this));
    }

    int metaTypeId() const { return _data ? _data->metaTypeId : QMetaType::UnknownType; }

protected:
    PersistentId(
            int metaTypeId,
            QString key,
            QHash<QString, QVariant> metadata
        )
        : _data(new PersistentIdData(metaTypeId, key, metadata))
    {
    }
    inline const QHash<QString, QVariant>& getMetadata() { return _data->metadata; }

private:
    QSharedDataPointer<const PersistentIdData> _data;

    friend uint qHash(const PersistentId& id, uint seed);
};

#define PERSISTENT_ID_BOILERPLATE(T) \
public: \
    T() = default;\
    T(const PersistentId& other);\
    inline operator QVariant() const { return QVariant::fromValue(*this); } \
    inline operator IndexVariant() const { return IndexVariant(QVariant::fromValue(*this)); } \
private: \
    T(QString key, QHash<QString, QVariant> metadata);

#define STATIC_PERSISTENT_ID_BOILERPLATE(T) \
    T::T(const PersistentId& other) : PersistentId(other.metaTypeId() == qMetaTypeId<T>() ? other : PersistentId()) {} \
    T::T(QString key, QHash<QString, QVariant> metadata) : PersistentId(qMetaTypeId<T>(), key, metadata) {}
    
Q_DECLARE_METATYPE(PersistentId);
Q_DECLARE_TYPEINFO(PersistentId, Q_MOVABLE_TYPE);

#endif // PERSISTENTID_HPP