/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef TYPEINFOREF_HPP
#define TYPEINFOREF_HPP

#include "compat.hpp"
#include <QObject>
#include <QVariant>
#include <typeinfo>
#include <typeindex>
namespace Addle {

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
	return QT_PREPEND_NAMESPACE(qHash(((const std::type_index&)ref).hash_code(), seed)); }


} // namespace Addle

Q_DECLARE_METATYPE(Addle::TypeInfoRef)

#endif // TYPEINFOREF_HPP