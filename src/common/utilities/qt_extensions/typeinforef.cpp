#include "typeinforef.hpp"

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