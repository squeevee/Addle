#ifndef QOBJECT_INTERFACE_TRAIT_H
#define QOBJECT_INTERFACE_TRAIT_H

#include <QObject>

#include <type_traits>

template<class Interface>
struct implemented_as_QObject : std::false_type {};

/**
 * @def DECL_IMPLEMENTED_AS_QOBJECT
 * @brief
 * Convenience macro declaring that `Interface` has trait implemented_as_QObject,
 * and registering `Interface` as an interface in Qt.
 * 
 * @param Interface
 * The interface being declared with the trait and registered.
 */
#define DECL_IMPLEMENTED_AS_QOBJECT(Interface) template<> struct implemented_as_QObject<Interface> : std::true_type {}; Q_DECLARE_INTERFACE(Interface, #Interface)

#endif //QOBJECT_INTERFACE_TRAIT_H