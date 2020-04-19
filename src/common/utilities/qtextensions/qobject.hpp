/**
 * Addle source code
 * @file qobject.h
 * @copyright Copyright 2019 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 * 
 * Specifies various general-purpose extensions to QObject functionality
 */
#ifndef QTEXCEPTIONS__QOBJECT_HPP
#define QTEXCEPTIONS__QOBJECT_HPP

#include <QObject>

#include "interfaces/traits/qobject_trait.hpp"

/**
 * @brief Wrapper for dynamic_cast<QObject*>(object), which requires
 * at compile-time for `Interface` to be implemented as a QObject.
 * @param object
 * The object to be cast
 */
template<class Interface>
inline QObject* qobject_interface_cast(Interface* object)
{
    static_assert(
        implemented_as_QObject<Interface>::value,
        "qobject_interface_cast may only be done with interfaces for QObjects"
    );

    return dynamic_cast<QObject*>(object);
}

/**
 * @brief Wrapper for dynamic_cast<QObject*>(object), which requires
 * at compile-time for `Interface` to be implemented as a QObject.
 * @param object
 * The object to be cast
 */
template<class Interface>
inline const QObject* qobject_interface_cast(const Interface* object)
{
    static_assert(
        implemented_as_QObject<Interface>::value,
        "qobject_interface_cast may only be done with interfaces for QObjects"
    );

    return dynamic_cast<const QObject*>(object);
}

/**
 * @brief Wrapper for QObject::connect that automatically converts from
 * interface types as necessary.
 */
template<class Interface>
    //typename std::enable_if<std::conditional<std::is_base_of<QObject, Interface>::value, std::false_type, std::true_type>::value>::type* = nullptr>
inline QMetaObject::Connection connect_interface(
        const Interface* sender,
        const char* signal,
        const QObject* receiver,
        const char* slot,
        Qt::ConnectionType type = Qt::AutoConnection
    )
{
    static_assert(
        implemented_as_QObject<Interface>::value,
        "connect_interface may only be done with interfaces for QObjects"
    );

    const QObject* qsender = dynamic_cast<const QObject*>(sender);

    //dynamic assert

    return QObject::connect(qsender, signal, receiver, slot, type);
}

template<class Interface>
    //typename std::enable_if<std::conditional<std::is_base_of<QObject, Interface>::value, std::false_type, std::true_type>::value>::type* = nullptr>
inline QMetaObject::Connection connect_interface(
        const QObject* sender,
        const char* signal,
        const Interface* receiver,
        const char* slot,
        Qt::ConnectionType type = Qt::AutoConnection
    )
{
    static_assert(
        implemented_as_QObject<Interface>::value,
        "connect_interface may only be done with interfaces for QObjects"
    );

    const QObject* qreceiver = dynamic_cast<const QObject*>(receiver);

    //todo: you can't virtually inherit QObject, so static_cast should probably
    //work here.
    //dynamic assert

    return QObject::connect(sender, signal, qreceiver, slot, type);
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
    static_assert(
        implemented_as_QObject<SenderInterface>::value,
        "connect_interface may only be done with interfaces for QObjects"
    );
    static_assert(
        implemented_as_QObject<ReceiverInterface>::value,
        "connect_interface may only be done with interfaces for QObjects"
    );

    const QObject* qsender = dynamic_cast<const QObject*>(sender);
    const QObject* qreceiver = dynamic_cast<const QObject*>(receiver);

    //todo: you can't virtually inherit QObject, so static_cast should probably
    //work here.
    //dynamic asserts

    return QObject::connect(qsender, signal, qreceiver, slot);
}

#endif // QTEXCEPTIONS__QOBJECT_HPP