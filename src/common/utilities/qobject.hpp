/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef QOBJECT_HPP
#define QOBJECT_HPP

#include <QCoreApplication>
#include <QObject>
#include <QPointer>

#include "interfaces/iamqobject.hpp"

// template<>
// inline QObject* qobject_cast<QObject*>(Addle::IAmQObject* object)
// {
//     return object->asQObject_p();
// }
// 
// template<>
// inline const QObject* qobject_cast<const QObject*>(const Addle::IAmQObject* object)
// {
//     return object->asQObject_p();
// }

namespace Addle {
    
template<class OutType, class InType>
inline OutType qobject_interface_cast(InType* object)
{
    return qobject_cast<OutType>(static_cast<IAmQObject*>(object)->asQObject_p());
}

template<class OutType, class InType>
inline OutType qobject_interface_cast(const InType* object)
{
    return qobject_cast<OutType>(static_cast<const IAmQObject*>(object)->asQObject_p());
}

template<class OutType, class InType>
inline QSharedPointer<OutType> qobject_interface_cast(QSharedPointer<InType> object)
{
    if (qobject_interface_cast<OutType*>(object.data()))
        return qSharedPointerCast<OutType>(object);
    else
        return nullptr;
}

template<class OutType, class InType>
inline QSharedPointer<const OutType> qobject_interface_cast(QSharedPointer<const InType> object)
{
    if (qobject_interface_cast<const OutType*>(object.data()))
        return qSharedPointerCast<const OutType>(object);
    else
        return nullptr;
}

/**
 * @brief Connect a QObject to a signal on an interface.
 */
template<class Interface>
inline QMetaObject::Connection connect_interface(
        const Interface* sender,
        const char* signal,
        const QObject* receiver,
        const char* slot,
        Qt::ConnectionType type = Qt::AutoConnection
    )
{
    return QObject::connect(qobject_interface_cast<const QObject*>(sender), signal, receiver, slot, type);
}

/**
 * @brief Connect a QObject to a slot on an interface.
 */
template<class Interface>
inline QMetaObject::Connection connect_interface(
        const QObject* sender,
        const char* signal,
        const Interface* receiver,
        const char* slot,
        Qt::ConnectionType type = Qt::AutoConnection
    )
{
    return QObject::connect(sender, signal, qobject_interface_cast<const QObject*>(receiver), slot, type);
}

/**
 * @brief Connect a QObject to a signal on an interface.
 */
template<class Interface>
inline QMetaObject::Connection connect_interface(
        QSharedPointer<const Interface> sender,
        const char* signal,
        const QObject* receiver,
        const char* slot,
        Qt::ConnectionType type = Qt::AutoConnection
    )
{
    return connect_interface(sender.data(), signal, receiver, slot, type);
}

/**
 * @brief Connect a QObject to a signal on an interface.
 */
template<class Interface>
inline QMetaObject::Connection connect_interface(
        QSharedPointer<Interface> sender,
        const char* signal,
        const QObject* receiver,
        const char* slot,
        Qt::ConnectionType type = Qt::AutoConnection
    )
{
    return connect_interface(sender.data(), signal, receiver, slot, type);
}

/**
 * @brief Connect a QObject to a slot on an interface.
 */
template<class Interface>
inline QMetaObject::Connection connect_interface(
        const QObject* sender,
        const char* signal,
        QSharedPointer<const Interface> receiver,
        const char* slot,
        Qt::ConnectionType type = Qt::AutoConnection
    )
{
    return connect_interface(sender, signal, receiver.data(), slot, type);
}

/**
 * @brief Connect a QObject to a slot on an interface.
 */
template<class Interface>
inline QMetaObject::Connection connect_interface(
        const QObject* sender,
        const char* signal,
        QSharedPointer<Interface> receiver,
        const char* slot,
        Qt::ConnectionType type = Qt::AutoConnection
    )
{
    return connect_interface(sender, signal, receiver.data(), slot, type);
}

template<class SenderInterface, 
    //typename std::enable_if<std::conditional<std::is_base_of<QObject, SenderInterface>::value, std::false_type, std::true_type>::type::value>::type* = nullptr,
    class ReceiverInterface>
    //typename std::enable_if<std::conditional<std::is_base_of<QObject, ReceiverInterface>::value, std::false_type, std::true_type>::type::value>::type* = nullptr
//TODO: it should be possible to get all these to cooperate overloading the same
//name by some SFINAE trick I haven't figured out yet.
inline QMetaObject::Connection connect_interface2(
        const SenderInterface* sender,
        const char* signal,
        const ReceiverInterface* receiver,
        const char* slot
    )
{
    const QObject* qsender = qobject_interface_cast<QObject*>(sender);
    const QObject* qreceiver = qobject_interface_cast<QObject*>(receiver);

    return QObject::connect(qsender, signal, qreceiver, slot);
}

template<class Interface>
inline bool sendInterfaceEvent(Interface* receiver, QEvent* event)
{
    return QCoreApplication::sendEvent(qobject_interface_cast<QObject*>(receiver), event);
}

template<class Interface>
inline bool sendInterfaceEvent(QSharedPointer<Interface> receiver, QEvent* event)
{
    return sendInterfaceEvent(receiver.data(), event);
}

template<class Interface>
inline void postInterfaceEvent(Interface* receiver, QEvent* event, int priority = Qt::NormalEventPriority)
{
    QCoreApplication::postEvent(qobject_interface_cast<QObject*>(receiver), event, priority);
}

template<class Interface>
inline void postInterfaceEvent(QSharedPointer<Interface> receiver, QEvent* event, int priority = Qt::NormalEventPriority)
{
    postInterfaceEvent(receiver.data(), event, priority);
}

template<class Interface>
class InterfacePointer
{
    static_assert(
       Traits::implemented_as_QObject<Interface>::value,
       "InterfacePointer may only be used for interfaces implemented as QObject"
    );
    typedef typename std::conditional<
        std::is_const<Interface>::value,
        const QObject,
        QObject
    >::type ObjectType;
    
public:
    inline InterfacePointer(Interface* interface = nullptr)
        : _inner(qobject_interface_cast<ObjectType>(interface))
    {
    }
    
    inline Interface& operator*() const { return *qobject_cast<Interface*>(_inner.data()); }
    
    inline Interface* data() const { return qobject_cast<Interface*>(_inner.data()); }
    inline Interface* operator->() const { return qobject_cast<Interface*>(_inner.data()); }
    
private:
    QPointer<ObjectType> _inner;
};

template<class OutType, class InType>
inline InterfacePointer<OutType> qobject_interface_cast(InterfacePointer<InType> ptr)
{
    return InterfacePointer<OutType>(qobject_interface_cast<OutType*>(ptr.data()));
}

} // namespace Addle

#endif // QOBJECT_HPP
