#ifndef AddleId_HPP
#define AddleId_HPP

#include "compat.hpp"
/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QSharedPointer>

#include <QHash>
#include <QUuid>
#include <QVariant>

#include "utilities/indexvariant.hpp"
namespace Addle {

constexpr QUuid ADDLE_NAMESPACE_UUID
    = QUuid(0xa6edc906, 0x2f9f, 0x5fb2, 0xa3, 0x73, 0xef, 0xac, 0x40, 0x6f, 0x0e, 0xf2);
    //{a6edc906-2f9f-5fb2-a373-efac406f0ef2}

template<typename IdType, quintptr Id>
class StaticIdMetaData;

/**
 * @class AddleId
 * @brief General-purpose (base class) identifier type.
 * AddleId is lightweight (same size as a pointer), and can be defined constexpr
 * and used in switch statements. It also has an extensible metadata mechanism
 * and can be safely downcast (to well-formed derived types).
 * 
 * AddleId's internal value is only unique within a single running instance of
 * Addle. String key or UUID metadata should be used for serialization, etc.
 */
class ADDLE_COMMON_EXPORT AddleId
{
public:
    static constexpr quintptr BEGIN_DYNAMIC_VALUES = 0x10000;

    constexpr AddleId(quintptr value = 0)
        : _value(value)
    { 
    }
    constexpr AddleId(const AddleId& other)
        : _value(other._value)
    {
    }

    inline const char* key() const { return isValid() ? baseMetaData().key : nullptr; }
    inline QUuid uuid() const { return isValid() ? baseMetaData().uuid : QUuid(); }
    inline int metaTypeId() const { return isValid() ? baseMetaData().metaTypeId : QMetaType::UnknownType; }

    constexpr inline bool isNull() const { return !_value; }
    explicit constexpr inline operator bool() const { return !isNull(); }
    constexpr inline bool operator!() const { return isNull(); }

    inline constexpr bool operator==(const AddleId& other) const
    {
        return _value == other._value;
    }
    inline constexpr bool operator!=(const AddleId& other) const
    {
        return _value != other._value;
    }

    inline constexpr operator quintptr() const { return _value; }

    explicit inline operator QVariant() const { return asQVariant_p(); }
    explicit inline operator IndexVariant() const { return IndexVariant(asQVariant_p()); }

    inline bool isValid() const { return !isNull() && _dynamicMetaData.contains(*this); }

protected:
    struct BaseMetaData
    {
        inline BaseMetaData(const char* const key_, int metaTypeId_, QUuid uuid_)
            : key(key_), metaTypeId(metaTypeId_), uuid(uuid_)
        {
            if (key && uuid.isNull())
            {
                uuid = QUuid::createUuidV5(ADDLE_NAMESPACE_UUID, QByteArray(key));
            }
            else if (!key && !uuid.isNull())
            {
                _uuidChars = uuid.toByteArray(QUuid::WithBraces);
                key = _uuidChars.constData();
            }
        }
        virtual ~BaseMetaData() = default;

        const char* key;
        int metaTypeId;
        QUuid uuid;
    private:
        QByteArray _uuidChars;
    };

    const BaseMetaData& baseMetaData() const { return *_dynamicMetaData.value(*this); }

private:
    quintptr _value;

    static QHash<AddleId, QSharedPointer<const BaseMetaData>> _dynamicMetaData;

    template<typename IdType, quintptr Id>
    friend class StaticIdMetaData;

    inline QVariant asQVariant_p() const
    {
        if (isValid())
            return QVariant((QVariant::Type)baseMetaData().metaTypeId, (void*)&_value);
        else
            return QVariant::fromValue(*this);
    }

    friend constexpr uint qHash(const AddleId& id, uint seed = 0)
    {
        return ::qHash(id._value, seed);
    }
    friend class IdInfo;
};

#define ID_TYPE_BOILERPLATE(T) \
public: \
    constexpr inline T(quintptr value = 0) : AddleId(value) {} \
    constexpr inline T(const T& other) : AddleId((quintptr)other) {} \
    inline T(const AddleId& other); \
    inline operator QVariant() const { return QVariant::fromValue(*this); } \
    inline operator IndexVariant() const { return IndexVariant(QVariant::fromValue(*this)); }

#define ID_TYPE_BOILERPLATE2(T) \
namespace Addle { inline T::T(const AddleId& other) : AddleId(!other.isNull() && other.metaTypeId() == qMetaTypeId<T>() ? (quintptr)other : 0) {} }

#define DECLARE_STATIC_ID_METADATA(id, key_) \
template<> class StaticIdMetaData<std::remove_const<decltype(id)>::type, static_cast<quintptr>(id)> { \
public: \
    static constexpr const char* key = key_; \
private: \
    static const QSharedPointer<const AddleId::BaseMetaData> _metaData; \
    friend class AddleId; \
};

#define GET_STATIC_ID_METADATA(id) \
StaticIdMetaData<std::remove_const<decltype(id)>::type, static_cast<quintptr>(id)>

} // namespace Addle

Q_DECLARE_METATYPE(Addle::AddleId);
Q_DECLARE_TYPEINFO(Addle::AddleId, Q_PRIMITIVE_TYPE);

#endif // AddleId_HPP
