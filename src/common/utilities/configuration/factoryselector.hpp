#ifndef FACTORYSELECTOR_HPP
#define FACTORYSELECTOR_HPP

#include <typeindex>
#include <functional>
#include <type_traits>
#include "idtypes/addleid.hpp"

#include "interfaces/iamqobject.hpp"
#include "utilities/qobject.hpp"

#include <QMetaType>
#include <QSharedPointer>
#include <QVariant>
#include <QList>

namespace Addle {
    
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
template<typename T>
struct _is_qt_metatype : std::integral_constant<bool, QMetaTypeId2<T>::Defined> {};
#else
#warning "Factory selection may not work properly for the current version of Qt"
template<typename T>
struct _is_qt_metatype : std::false_type {};
#endif
    
/**
 * @class FactorySelector
 * Used by ServiceLocator and IServiceConfig to choose a factory during a make 
 * operation.
 */
class FactorySelector
{
public:    
    inline FactorySelector(std::type_index interfaceType)
        : _interfaceType(interfaceType)
    {
    }
    
    inline FactorySelector(std::type_index interfaceType, AddleId id)
        : _interfaceType(interfaceType), _id(id)
    {
    }
    
    inline std::type_index interfaceType() const { return _interfaceType; }
    inline AddleId id() const { return _id; }
    
    inline bool hasArgs() const { return !_args.isEmpty(); }
    inline int argCount() const { return _args.count(); }
    
    inline FactorySelector withoutArgs() const { return FactorySelector(_interfaceType, _id); }
    
    inline QVariant argAt(int index) const { return _args.at(index); }

    template<class Interface>
    static inline FactorySelector fromArgs()
    {
        return fromInterfaceType<Interface>();
    }
    
    template<class Interface, typename Arg1_t> 
    static inline FactorySelector fromArgs(Arg1_t& arg1)
    {
        return fromInterfaceType<Interface>()
            .addArg(arg1);
    }
    
    template<class Interface, typename Arg1_t, typename Arg2_t> 
    static inline FactorySelector fromArgs(Arg1_t& arg1, Arg2_t& arg2)
    {
        return fromInterfaceType<Interface>()
            .addArg(arg1)
            .addArg(arg2);
    }
    
    template<class Interface, typename Arg1_t, typename Arg2_t, typename Arg3_t> 
    static inline FactorySelector fromArgs(Arg1_t& arg1, Arg2_t& arg2, Arg3_t& arg3)
    {
        return fromInterfaceType<Interface>()
            .addArg(arg1)
            .addArg(arg2)
            .addArg(arg3);
    }
    
    template<class Interface, typename Arg1_t, typename Arg2_t, typename Arg3_t, 
        typename Arg4_t> 
    static inline FactorySelector fromArgs(Arg1_t& arg1, Arg2_t& arg2, Arg3_t& arg3,
        Arg4_t& arg4)
    {
        return fromInterfaceType<Interface>()
            .addArg(arg1)
            .addArg(arg2)
            .addArg(arg3)
            .addArg(arg4);
    }
    
    template<class Interface, typename Arg1_t, typename Arg2_t, typename Arg3_t, 
        typename Arg4_t, typename Arg5_t> 
    static inline FactorySelector fromArgs(Arg1_t& arg1, Arg2_t& arg2, Arg3_t& arg3, 
        Arg4_t& arg4, Arg5_t& arg5)
    {
        return fromInterfaceType<Interface>()
            .addArg(arg1)
            .addArg(arg2)
            .addArg(arg3)
            .addArg(arg4)
            .addArg(arg5);
    }
    
    template<class Interface, typename Arg1_t, typename Arg2_t, typename Arg3_t,
        typename Arg4_t, typename Arg5_t, typename Arg6_t> 
    static inline FactorySelector fromArgs(Arg1_t& arg1, Arg2_t& arg2, Arg3_t& arg3,
        Arg4_t& arg4, Arg5_t& arg5, Arg6_t& arg6)
    {
        return fromInterfaceType<Interface>()
            .addArg(arg1)
            .addArg(arg2)
            .addArg(arg3)
            .addArg(arg4)
            .addArg(arg5)
            .addArg(arg6);
    }
    
    template<class Interface, typename Arg1_t, typename Arg2_t, typename Arg3_t,
        typename Arg4_t, typename Arg5_t, typename Arg6_t, typename Arg7_t> 
    static inline FactorySelector fromArgs(Arg1_t& arg1, Arg2_t& arg2, Arg3_t& arg3,
        Arg4_t& arg4, Arg5_t& arg5, Arg6_t& arg6, Arg7_t& arg7)
    {
        return fromInterfaceType<Interface>()
            .addArg(arg1)
            .addArg(arg2)
            .addArg(arg3)
            .addArg(arg4)
            .addArg(arg5)
            .addArg(arg6)
            .addArg(arg7);
    }
    
    template<class Interface, typename Arg1_t, typename Arg2_t, typename Arg3_t,
        typename Arg4_t, typename Arg5_t, typename Arg6_t, typename Arg7_t, typename Arg8_t> 
    static inline FactorySelector fromArgs(Arg1_t& arg1, Arg2_t& arg2, Arg3_t& arg3,
        Arg4_t& arg4, Arg5_t& arg5, Arg6_t& arg6, Arg7_t& arg7, Arg8_t& arg8)
    {
        return fromInterfaceType<Interface>()
            .addArg(arg1)
            .addArg(arg2)
            .addArg(arg3)
            .addArg(arg4)
            .addArg(arg5)
            .addArg(arg6)
            .addArg(arg7)
            .addArg(arg8);
    }
    
    
    FactorySelector& addArg(QObject* arg)
    {
        _args.append(QVariant::fromValue(arg));
        return *this;
    }
    
    FactorySelector& addArg(QObject& arg)
    {
        _args.append(QVariant::fromValue(&arg));
        return *this;
    }
    
    FactorySelector& addArg(QSharedPointer<QObject> arg)
    {
        _args.append(QVariant::fromValue(arg.data()));
        return *this;
    }
    
    template<class Interface>
    inline typename std::enable_if<
        Traits::implemented_as_QObject<typename std::remove_cv<Interface>::type>::value
        && !std::is_convertible<typename std::remove_cv<Interface>::type*, QObject*>::value,
        FactorySelector&
    >::type addArg(QSharedPointer<Interface> arg)
    {
        _args.append(
            QVariant::fromValue(qobject_interface_cast<QObject*>(
                const_cast<typename std::remove_cv<Interface>::type*>(arg.data())
            ))
        );
        return *this;
    }
    
    template<class Interface>
    inline typename std::enable_if<
        Traits::implemented_as_QObject<typename std::remove_cv<Interface>::type>::value
        && !std::is_convertible<typename std::remove_cv<Interface>::type*, QObject*>::value,
        FactorySelector&
    >::type addArg(Interface* arg)
    {
        _args.append(
            QVariant::fromValue(qobject_interface_cast<QObject*>(
                const_cast<typename std::remove_cv<Interface>::type*>(arg)
            ))
        );
        return *this;
    }
    
    template<class Interface>
    inline typename std::enable_if<
        Traits::implemented_as_QObject<typename std::remove_cv<Interface>::type>::value
        && !std::is_convertible<typename std::remove_cv<Interface>::type*, QObject*>::value,
        FactorySelector&
    >::type addArg(Interface& arg)
    {
        _args.append(
            QVariant::fromValue(qobject_interface_cast<QObject*>(
                const_cast<typename std::remove_cv<Interface>::type*>(&arg)
            ))
        );
        return *this;
    }
    
    template<typename T>
    inline typename std::enable_if<
        !Traits::implemented_as_QObject<T>::value
        && _is_qt_metatype<T>::value,
        FactorySelector&
    >::type addArg(const T& arg)
    {
        _args.append(QVariant::fromValue<T>(arg));
        return *this;
    }
    
    template<typename T>
    inline typename std::enable_if<
        !Traits::implemented_as_QObject<T>::value
        && !_is_qt_metatype<T>::value,
        FactorySelector&
    >::type addArg(const T&)
    {
        _args.append(QVariant());
        return *this;
    }
    
    template<typename T>
    inline FactorySelector& addArg(std::reference_wrapper<T> r)
    {
        addArg(static_cast<T&>(r));
        return *this;
    }

    template<class Interface>
    static inline FactorySelector fromInterfaceType()
    {
        return FactorySelector(std::type_index(typeid(Interface)));
    }
    
private:
    std::type_index _interfaceType;
    AddleId _id;
    QList<QVariant> _args;
};
    
} // namespace Addle

#endif // FACTORYSELECTOR_HPP
