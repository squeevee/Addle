#ifndef OPTIONALMETAOBJECT_HPP
#define OPTIONALMETAOBJECT_HPP

#include <QMetaObject>

namespace Addle {

/**
 * Exposes a pointer to the static QMetaObject for a type or a null pointer if
 * the type has no static QMetaObject.
 */ 
template<class T>
class OptionalMetaObject
{
    template<class>
    static constexpr const QMetaObject* metaObject_p(long) { return nullptr; }
    
    template<class T_, decltype(T_::staticMetaObject)>
    static constexpr const QMetaObject* metaObject_p(int) { return static_cast<const QMetaObject*>(&T_::staticMetaObject); }
public:
    OptionalMetaObject() = delete;
    OptionalMetaObject(const OptionalMetaObject&) = delete;
    
    static constexpr const QMetaObject* const metaObject = metaObject_p<T>(0);
};

} // namespace Addle

#endif // OPTIONALMETAOBJECT_HPP
