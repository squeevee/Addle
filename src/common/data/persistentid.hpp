#ifndef PERSISTENTID_HPP
#define PERSISTENTID_HPP

#include <QSharedData>
#include <QSharedDataPointer>
#include <QHash>
#include <QVariant>
#include <typeinfo>

/**
 * @class PersistentId
 * @brief Base class for identifier objects that can be used to represent
 * various concepts in Addle
 * 
 * PersistentId types and objects represent various concepts that may not be
 * fully, but which are expected to be created early in runtime and remain
 * relatively constant.
 * 
 * See derived classes for examples.
 * 
 * In many cases PersistentId objects are exposed as public static const variables.
 * These may be copied, added to collections, or used as "canonical" instances
 * for comparison as necessary. PersistentId objects may also be generated during 
 * runtime as appropriate, for example by a plugin host.
 * 
 * PersistentId objects are only considered equal if they are copies of the same
 * initial instance, even if they contain identical data. To this end, they use
 * Qt's implicit sharing for fast copying and comparison. They can be used as
 * keys in a QHash, as values in any collection, are nullable, and may be
 * converted to or (explicitly) from a QVariant.
 * 
 * All PersistentId objects have a short QString "key". This can be useful for
 * debugging but can also be used for referencing an id object in a translation
 * or settings file. Derived classes may specify additional metadata.
 */
class PersistentId
{
    struct PersistentIdData : QSharedData
    {
        PersistentIdData(QString key, QHash<QString, QVariant> metadata)
            : key(key), metadata(metadata)
        {
        }

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

    inline operator bool() const
    {
        return !isNull();
    }

    inline bool operator!() const
    {
        return isNull();
    }

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

protected:
    PersistentId(
            const char* key,
            QHash<QString, QVariant> metadata = QHash<QString, QVariant>()
        )
        : _data(new PersistentIdData(key, metadata))
    {
    }

    inline const QHash<QString, QVariant>& getMetadata() { return _data->metadata; }

private:
    QSharedDataPointer<const PersistentIdData> _data;

    friend uint qHash(const PersistentId& id);
};

Q_DECLARE_METATYPE(PersistentId)

#endif // PERSISTENTID_HPP