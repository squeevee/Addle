/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IAMQOBJECT_HPP
#define IAMQOBJECT_HPP

#include <QObject>
#include <type_traits>

namespace Addle {

/**
 * @brief A special interface that indicates its implementation is a QObject.
 * Use qobject_cast to access the underlying QObject (or to safely cast to 
 * another type supported by qobject_cast).
 * 
 * Interfaces that want to expose QObject features should publicly (virtually)
 * inherit IAmQObject. Classes implementing this should do so with the
 * convenience macro IAMQOBJECT_IMPL
 */
class IAmQObject
{
public:
    virtual ~IAmQObject() = default;
    
    IAmQObject& operator=(const IAmQObject&) = delete;
    IAmQObject& operator=(IAmQObject&&) = delete;
    
protected:
    virtual QObject* asQObject_p() = 0;
    virtual const QObject* asQObject_p() const = 0;

    template<class OutType, class InType>
    friend OutType qobject_interface_cast(InType* object);
    
    template<class OutType, class InType>
    friend OutType qobject_interface_cast(const InType* object);
};

/**
 * @def 
 * @brief Place this macro in the implementation of an IAmQbject interface.
 */
#define IAMQOBJECT_IMPL \
protected: \
    QObject* asQObject_p() override { return this; } \
    const QObject* asQObject_p() const override { return this; } \
private:

namespace Traits {

template<class Interface>
struct implemented_as_QObject : std::integral_constant<bool, std::is_base_of<IAmQObject, Interface>::value> {};

} // namespace Traits

} // namespace Addle

#endif // IAMQOBJECT_HPP
