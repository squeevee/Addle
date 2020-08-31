/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef QOBJECT_HPP
#define QOBJECT_HPP

#include <QObject>
#include <QPointer>

#include "interfaces/iamqobject.hpp"
namespace Addle {

/**
 * @brief Acquire QObject from Interface
 */
template<class Interface>
typename std::enable_if<
    !std::is_const<Interface>::value,
    QObject*
>::type qobject_interface_cast(Interface* object)
{
    return static_cast<IAmQObject*>(object)->asQObject_p();
}

/**
 * @brief Acquire QObject from Interface
 */
template<class Interface>
typename std::enable_if<
    !std::is_const<Interface>::value,
    const QObject*
>::type qobject_interface_cast(const Interface* object)
{
    return static_cast<const IAmQObject*>(object)->asQObject_p();
}

template<class Interface>
QObject* qobject_interface_cast(QSharedPointer<Interface> object)
{
    return qobject_interface_cast(object.data());
}

template<class Interface>
const QObject* qobject_interface_cast(QSharedPointer<const Interface> object)
{
    return qobject_interface_cast(object.data());
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
    const QObject* qsender = qobject_interface_cast(sender);
    return QObject::connect(qsender, signal, receiver, slot, type);
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
    const QObject* qreceiver = qobject_interface_cast<Interface>(receiver);
    return QObject::connect(sender, signal, qreceiver, slot, type);
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
    // static_assert(
    //     implemented_as_QObject<SenderInterface>::value,
    //     "connect_interface may only be done with interfaces for QObjects"
    // );
    // static_assert(
    //     implemented_as_QObject<ReceiverInterface>::value,
    //     "connect_interface may only be done with interfaces for QObjects"
    // );

    const QObject* qsender = qobject_interface_cast(sender);//dynamic_cast<const QObject*>(sender);
    const QObject* qreceiver = qobject_interface_cast(receiver);//dynamic_cast<const QObject*>(receiver);


    //dynamic asserts

    return QObject::connect(qsender, signal, qreceiver, slot);
}
} // namespace Addle

#endif // QOBJECT_HPP