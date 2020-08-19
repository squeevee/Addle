/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "typeinforef.hpp"
#include <typeindex>

using namespace Addle;

TypeInfoRef::TypeInfoRef()
    : _typeinfo_ptr(nullptr)
{
}

TypeInfoRef::TypeInfoRef(const std::type_info& typeinfo)
    : _typeinfo_ptr(&typeinfo)
{
}

TypeInfoRef::TypeInfoRef(const TypeInfoRef& other)
    : _typeinfo_ptr(other._typeinfo_ptr)
{
}

TypeInfoRef::operator const std::type_info &() const
{
    //assert _typeinfo_ptr is not null

    return *_typeinfo_ptr;
}

TypeInfoRef::operator QVariant() const
{
    QVariant v;
    v.setValue(*this);
    return v;
}