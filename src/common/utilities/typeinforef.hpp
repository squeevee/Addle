#ifndef TYPEINFOREF_HPP
#define TYPEINFOREF_HPP

#include <QObject>
#include <QVariant>
#include <typeinfo>
#include <typeindex>
#include "interfaces/traits/compat.hpp"

/**
 * @class TypeInfoRef
 * @brief Wrapper class for a reference to std::type_info, for easier
 * compatibility with Qt's meta-object system.
 */
class ADDLE_COMMON_EXPORT TypeInfoRef
{
public:
    TypeInfoRef();
    TypeInfoRef(const std::type_info& typeinfo);
    TypeInfoRef(const TypeInfoRef& other);

    operator const std::type_info&() const;
    operator QVariant() const;

private:
    const std::type_info* _typeinfo_ptr;
};

inline uint qHash(const TypeInfoRef& ref, uint seed = 0)
{
	return ((const std::type_index&)ref).hash_code() ^ seed; 
}

Q_DECLARE_METATYPE(TypeInfoRef)

#endif // TYPEINFOREF_HPP