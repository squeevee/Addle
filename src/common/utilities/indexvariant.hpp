#ifndef INDEXVARIANT_HPP
#define INDEXVARIANT_HPP

#include <QHash>
#include <QMetaType>
#include <QVariant>

#include <typeindex>
#include <type_traits>

#include "typeinforef.hpp"

/**
 * @class IndexVariant
 * @brief QHash-compatible wrapper for QVariant holding a hashable (i.e., index)
 * value.
 * 
 * @note Use direct conversions between QVariant and IndexVariant, not meta
 * conversions.
 */
class IndexVariant
{
public:
    IndexVariant() = default;
    IndexVariant(const IndexVariant& other) = default;
    IndexVariant(const QVariant& var);

    uint getHash(uint seed = 0) const;

    inline bool operator==(const IndexVariant& rhs) const
    {
        return _var == rhs._var;
    }

    inline bool operator==(const QVariant& rhs) const
    {
        return _var == rhs;
    }

    inline bool operator!=(const IndexVariant& rhs) const
    {
        return !(*this == rhs);
    }

    inline bool operator!=(const QVariant& rhs) const
    {
        return !(*this == rhs);
    }

    inline operator QVariant() const
    {
        return _var;
    }

    static bool variantCanHash(const QVariant& var);

    template<typename T>
    static IndexVariant fromValue(T value)
    {
        return IndexVariant(QVariant::fromValue(value));
    }

    /**
     * @note Calling this function on a type for which qHash() is not implemented
     * will result in a linker error.
     */
    template<typename TRegister, typename THasher = TRegister>
    static void registerHasher()
    {
        //TODO: async safety
        //TODO: static assert

        _hashers.insert(qMetaTypeId<TRegister>(), &hasher<THasher>);
    }

private:
    QVariant _var;

    template<typename T>
    static uint hasher(QVariant var, uint seed)
    {
        return qHash(var.value<T>(), seed);
    }

    static QHash<int, uint(*)(QVariant, uint)> _hashers;

    friend bool operator==(const QVariant& lhs, const IndexVariant& rhs);
};

inline bool operator==(const QVariant& lhs, const IndexVariant& rhs)
{
    return lhs == rhs._var;
}

inline bool operator!=(const QVariant& lhs, const IndexVariant& rhs)
{
    return !(lhs == rhs);
}

uint qHash(const IndexVariant& var, uint seed = 0);

Q_DECLARE_METATYPE(IndexVariant)

typedef QHash<IndexVariant, QVariant> IndexVariantHash;

#endif // INDEXVARIANT_HPP