#ifndef IAMQOBJECT_HPP
#define IAMQOBJECT_HPP

#include <QObject>
#include <type_traits>

namespace Addle {


template<class Interface>
typename std::enable_if<
    !std::is_const<Interface>::value,
    QObject*
>::type qobject_interface_cast(Interface* object);

template<class Interface>
typename std::enable_if<
    !std::is_const<Interface>::value,
    const QObject*
>::type qobject_interface_cast(const Interface* object);

class IAmQObject
{
protected:
    virtual QObject* asQObject() = 0;
    virtual const QObject* asQObject() const = 0;

    template<class Interface>
    friend typename std::enable_if<
        !std::is_const<Interface>::value,
        QObject*
    >::type qobject_interface_cast(Interface* object);

    template<class Interface>
    friend typename std::enable_if<
        !std::is_const<Interface>::value,
        const QObject*
    >::type qobject_interface_cast(const Interface* object);

};

#define IAMQOBJECT_IMPL \
protected: \
    QObject* asQObject() override { return this; } \
    const QObject* asQObject() const override { return this; } \
private:

template<class Interface>
struct implemented_as_QObject : std::integral_constant<bool, std::is_base_of<IAmQObject, Interface>::value> {};

} // namespace Addle

#endif // IAMQOBJECT_HPP