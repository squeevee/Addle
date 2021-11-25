#pragma once

#include <unordered_map>
#include <stdexcept>
#include <cstring>
#include <cstdlib>  // std::strlen, std::memcpy
#include <optional>

#include <boost/mp11.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/type_traits.hpp>
#include <boost/type_traits/is_detected_convertible.hpp>

#include <QObject>
#include <QVariant>
#include <QHash>
#include <QMetaType>
#include <QSharedData>
#include <QAtomicPointer>
#include <QAtomicInteger>

#include <QCoreApplication>

#include "interfaces/traits.hpp"
#include "utilities/ranges.hpp"
#include "utilities/qobject.hpp"
#include "utilities/coordinatedreadwritelock.hpp"
#include "utilities/metaprogramming.hpp"

#include "./configexceptions.hpp"

#ifdef ADDLE_DEBUG
#include <QDebug> // operator<< for GenericRepoMemberRef_impl
#include "utilities/debugging/qdebug_extensions.hpp"
#endif


namespace Addle::aux_config3 {

using anonymous_unique_ptr = std::unique_ptr<void, void(*)(void*)>;
template<typename T>
void _anonymous_deleter_impl(void* obj_)
{
    delete static_cast<T*>(obj_);
}

template<typename T>
anonymous_unique_ptr make_anonymous_unique_ptr(T* obj)
{
    return anonymous_unique_ptr(obj, &_anonymous_deleter_impl<T>);
}

// struct _anonymous_atomic_deleter
// {
//     using pointer = QAtomicPointer<void>;
//     void operator()(QAtomicPointer<void> ptr) const { _inner(ptr.loadRelaxed()); }
//     void(*_inner)(void*);
// };
// 
// using anonymous_unique_atomic_ptr = std::unique_ptr<void, _anonymous_atomic_deleter>;
// 
// template<typename T>
// anonymous_unique_atomic_ptr make_anonymous_unique_atomic_ptr(QAtomicPointer<T> obj)
// {
//     return anonymous_unique_atomic_ptr(
//             obj, 
//             _anonymous_atomic_deleter { &_anonymous_deleter_impl<T> }
//         );
// }

template<class T>
using _validate_pair_t = decltype((std::declval<T>().first, std::declval<T>().second));

template<typename Key_, 
    bool = QMetaTypeId2<Key_>::Defined, 
    bool = std::is_pointer_v<Key_> && std::is_base_of_v<QObject, boost::remove_cv_ref_t<std::remove_pointer_t<Key_>> >,
    bool = std::is_pointer_v<Key_> && Traits::implemented_as_QObject< boost::remove_cv_ref_t<std::remove_pointer_t<Key_>> >::value>
struct RepoKeyVariantHelper
{
    static constexpr bool Supported = false;
};

template<typename Key_, bool _1, bool _2>
struct RepoKeyVariantHelper<Key_, true, _1, _2>
{
//     using intermediate_type = Key_;
    static constexpr bool Supported = true;
    static QMetaType::Type type() { return (QMetaType::Type)(qMetaTypeId<Key_>()); }
    
    static QVariant toVariant(const Key_& key) { return QVariant::fromValue(key); }
    static std::optional<Key_> fromVariant(QVariant v, bool silenceErrors = false)
    { 
#ifdef ADDLE_DEBUG
        QVariant original(v);
#endif
        bool success = v.convert(qMetaTypeId<Key_>());
        if (!silenceErrors && Q_UNLIKELY(!success))
        {
            ADDLE_THROW(GenericRepoVariantConversionFailure(
                    GenericRepoVariantConversionFailure::Key
#ifdef ADDLE_DEBUG
                    , original
                    , qMetaTypeId<Key_>()
#endif
                ));
        }
        
        return success ? std::optional<Key_>(v.value<Key_>()) : std::optional<Key_>();
    }
    
//     static Key_ toIntermediate(const Key_& key) { return key; }
//     static Key_ toIntermediate(Key_&& key) { return std::move(key); }
//     static std::optional<Key_> fromIntermediate(const Key_& key) { return key; }
//     static std::optional<Key_> fromIntermediate(Key_&& key) { return std::move(key); }
};

template<typename Key_, bool _>
struct RepoKeyVariantHelper<Key_*, false, true, _>
{
    // is this redundant? Pointers derived from QObject are automatically 
    // supported by QVariant -- this would permit const pointers but that's 
    // hardly that big a deal and maybe also fine with QVariant.
    
//     using intermediate_type = QObject*;
    static constexpr bool Supported = true;
    static QMetaType::Type type() { return QMetaType::QObjectStar; }
    
    static QVariant toVariant(Key_* key) 
    { return QVariant::fromValue(static_cast<QObject*>(const_cast<std::remove_const_t<Key_>*>(key))); }
    
    std::optional<Key_*> fromVariant(QVariant v, bool silenceErrors = true)
    {
#ifdef ADDLE_DEBUG
        QVariant original(v);
#endif
        bool success = v.convert(QMetaType::QObjectStar);
        auto obj = v.value<QObject*>();
        auto result = qobject_cast<Key_*>(obj);
        if (!silenceErrors &&  Q_UNLIKELY(!success || (obj && !result)))
        {
            ADDLE_THROW(GenericRepoVariantConversionFailure(
                    GenericRepoVariantConversionFailure::Key
#ifdef ADDLE_DEBUG
                    , original
                    , QMetaType::QObjectStar
#endif
                ));
        }
        return result;
    }
    
//     static QObject* toIntermediate(Key_* key)
//     { return static_cast<QObject*>(const_cast<std::remove_const_t<Key_>*>(key)); }
//     static std::optional<Key_*> fromIntermediate(QObject* i)
//     { 
//         auto result = qobject_cast<Key_*>(i); 
//         return ((bool)(result) == (bool)(i)) ? std::optional<Key_*>(result) : std::optional<Key_*>();
//     }
};

template<typename Key_>
struct RepoKeyVariantHelper<Key_*, false, false, true>
{    
//     using intermediate_type = QObject*;
    static constexpr bool Supported = true;
    static QMetaType::Type type() { return QMetaType::QObjectStar; }
    
    static QVariant toVariant(Key_* key) 
    {
        return QVariant::fromValue(
            qobject_interface_cast<QObject*>(const_cast<std::remove_const_t<Key_>*>(key))
        );
    }
    
    std::optional<Key_*> fromVariant(QVariant v, bool silenceErrors = false)
    {
#ifdef ADDLE_DEBUG
        QVariant original(v);
#endif
        
        bool success = v.convert(QMetaType::QObjectStar);
        auto obj = v.value<QObject*>();
        auto result = qobject_cast<std::remove_const_t<Key_>*>(obj);
        
        if (!silenceErrors &&  Q_UNLIKELY(!success || (obj && !result)))
        {
            ADDLE_THROW(GenericRepoVariantConversionFailure(
                    GenericRepoVariantConversionFailure::Key
#ifdef ADDLE_DEBUG
                    , original 
                    , QMetaType::QObjectStar
#endif
                ));
        }      
        
        return result;
    }
    
//     static QObject* toIntermediate(Key_* key)
//     { return qobject_interface_cast<QObject*>(const_cast<std::remove_const_t<Key_>*>(key)); }
//     static std::optional<Key_*> fromIntermediate(QObject* i)
//     { 
//         auto result = qobject_cast<std::remove_const_t<Key_>*>(i); 
//         return ((bool)(result) == (bool)(i)) ? std::optional<Key_*>(result) : std::optional<Key_*>();
//     }
};

template<typename Key_>
struct RepoKeyVariantHelper<Key_*, false, false, false>
{
//     using intermediate_type = void*;
    static constexpr bool Supported = true;
    static QMetaType::Type type() { return QMetaType::VoidStar; }
    static QVariant toVariant(Key_* key) 
    { return QVariant::fromValue(static_cast<void*>(const_cast<std::remove_const_t<Key_>*>(key))); }
    
    std::optional<Key_*> fromVariant(QVariant v, bool silenceErrors = false)
    {
#ifdef ADDLE_DEBUG
        QVariant original(v);
#endif
        bool success = v.convert(QMetaType::VoidStar);        
        if (!silenceErrors && Q_UNLIKELY(!success))
        {
            ADDLE_THROW(GenericRepoVariantConversionFailure(
                    GenericRepoVariantConversionFailure::Key
#ifdef ADDLE_DEBUG
                    , original
                    , QMetaType::VoidStar
#endif
                ));
        }
        
        return static_cast<Key_*>(v.value<void*>());
    }
    
//     void* toIntermediate(Key_* key)
//     { return static_cast<void*>(const_cast<std::remove_const_t<Key_>*>(key)); }
//     std::optional<Key_*> fromIntermediate(void* i)
//     { return static_cast<Key_*>(i); }
};

template<class Interface, 
        bool = QMetaTypeId2<QSharedPointer<Interface>>::Defined,
        bool = Traits::implemented_as_QObject<std::remove_cv_t<Interface>>::value
    >
struct RepoInterfaceVariantHelper
{
    static constexpr bool Supported = true;
    
    static QVariant toVariant(QSharedPointer<Interface> interface)
    {
        return QVariant::fromValue(interface);
    }
    
    static QSharedPointer<Interface> fromVariant(QVariant v, bool silenceErrors = false)
    {
#ifdef ADDLE_DEBUG
        QVariant original(v);
#endif
        
        auto success = v.convert(qMetaTypeId<QSharedPointer<Interface>>());

        if (!silenceErrors && Q_UNLIKELY(!success))
        {
            ADDLE_THROW(GenericRepoVariantConversionFailure(
                    GenericRepoVariantConversionFailure::Member
#ifdef ADDLE_DEBUG
                    , original
                    , qMetaTypeId<QSharedPointer<Interface>>()
#endif
                ));
        }
        
        return v.value<QSharedPointer<Interface>>();
    }
    
//     template<typename Key>
//     static QVariant pairToVariant(Key&& key, QSharedPointer<Interface> member)
//     {
//         return QVariant::fromValue(qMakePair(
//                 RepoKeyVariantHelper<boost::remove_cv_ref_t<Key>>::toIntermediate(std::forward<Key>(key)),
//                 member
//             ));
//     }
//     
//     template<typename Key>
//     static std::pair<Key, QSharedPointer<Interface>> pairFromVariant(QVariant v, bool silenceErrors = false)
//     {
// #ifdef ADDLE_DEBUG
//         QVariant original(v);
// #endif
//         
//         using intermediate_type = QPair<
//                 typename RepoKeyVariantHelper<Key>::intermediate_type,
//                 QSharedPointer<Interface>
//             >;
//             
//         auto success = v.convert(qMetaTypeId<intermediate_type>());
//         auto intermediate = v.value<intermediate_type>();
//         auto key = RepoKeyVariantHelper<Key>::fromIntermediate(intermediate.first);
//         
//         if (!silenceErrors && Q_UNLIKELY(!success || !key))
//         {
//             throw RepoVariantConversionFailure(
// #ifdef ADDLE_DEBUG
//                     original,
//                     qMetaTypeId<intermediate_type>()
// #endif
//                 );
//         }
//         
//         return (success && key) ?
//               std::pair<Key, QSharedPointer<Interface>>(*key, std::move(intermediate.second))
//             : std::pair<Key, QSharedPointer<Interface>>();
//     }
};

template<class Interface>
struct RepoInterfaceVariantHelper<Interface, false, true>
{
    static constexpr bool Supported = true;
    
    static QVariant toVariant(QSharedPointer<Interface> member)
    {
        return QVariant::fromValue(QtSharedPointer::copyAndSetPointer(
                const_cast<QObject*>(
                    qobject_interface_cast<QObject*>(member.data())
                ),
                member
            ));
    }
    
    static QSharedPointer<Interface> fromVariant(QVariant v, bool silenceErrors = false)
    {
#ifdef ADDLE_DEBUG
        QVariant original(v);
#endif
        auto success = v.convert(qMetaTypeId<QSharedPointer<QObject>>());
        
        auto obj    = v.value<QSharedPointer<QObject>>();
        auto result = obj.objectCast<Interface>();
                
        if (!silenceErrors && Q_UNLIKELY(!success || (obj && !result)))
        {
            ADDLE_THROW(GenericRepoVariantConversionFailure(
                    GenericRepoVariantConversionFailure::Member
#ifdef ADDLE_DEBUG
                    , original
                    , qMetaTypeId<QSharedPointer<QObject>>()
#endif
                ));
        }
        
        return result;
    }
    
//     template<typename Key>
//     static QVariant pairToVariant(Key&& key, QSharedPointer<Interface> member)
//     {
//         return QVariant::fromValue(qMakePair(
//                 RepoKeyVariantHelper<boost::remove_cv_ref_t<Key>>::toIntermediate(std::forward<Key>(key)),
//                 QtSharedPointer::copyAndSetPointer(
//                 const_cast<QObject*>(
//                     qobject_interface_cast<QObject*>(member.data())
//                 ),
//                 member
//             )));
//     }
//     
//     template<typename Key>
//     static std::pair<Key, QSharedPointer<Interface>> pairFromVariant(QVariant v, bool silenceErrors = false)
//     {
// #ifdef ADDLE_DEBUG
//         QVariant original(v);
// #endif
//         using intermediate_type = QPair<
//                 typename RepoKeyVariantHelper<Key>::intermediate_type,
//                 QSharedPointer<QObject>
//             >;
//             
//         auto success = v.convert(qMetaTypeId<intermediate_type>());
//         
//         auto intermediate = v.value<intermediate_type>();
//         auto key = RepoKeyVariantHelper<Key>::fromIntermediate(intermediate.first);
//         auto member = intermediate.second.template objectCast<Interface>();
//         
//         if (!silenceErrors && Q_UNLIKELY(!success || !key || (intermediate.second && !member)))
//         {
//             throw RepoVariantConversionFailure(
// #ifdef ADDLE_DEBUG
//                     original,
//                     qMetaTypeId<intermediate_type>()
// #endif
//                 );
//         }
//         
//         return (success && key && ((bool)(intermediate.second) == (bool)(member))) ? 
//               std::pair<Key, QSharedPointer<Interface>>(*key, std::move(member))
//             : std::pair<Key, QSharedPointer<Interface>>();
//     }
};

template<class Interface>
struct RepoInterfaceVariantHelper<Interface, false, false>
{
    static constexpr bool Supported = false;
};

// // Does nothing if `v` already has the type `T`, otherwise attempts to convert
// // the variant, and throws RepoVariantConversionFailure in the event of a failure.
// template<typename T>
// inline void conformType(QVariant& v)
// {
//     if (Q_LIKELY(v.type() != qMetaTypeId<T>()))
//         return;
// 
// #ifdef ADDLE_DEBUG
//     // Because QVariant::convert will destroy the original value in the event of
//     // a conversion failure, if we want to diagnose the failure, we have to make
//     // a copy of the value before converting. This should typically not be too
//     // expensive (at least not by QVariant standards), but we'll still reserve 
//     // it for debug builds only.
//     QVariant original(v);
// #endif
//     
//     if (Q_UNLIKELY(!v.convert(qMetaTypeId<T>())))
//     {
//         throw VariantConversionFailure(
// #ifdef ADDLE_DEBUG
//                 std::move(original)
// #endif
//             );
//     }
// }

template<class Interface>
using _qt_static_metaobject_t = decltype(Interface::staticMetaObject);

template<typename Interface, 
    bool = boost::mp11::mp_valid<_qt_static_metaobject_t, Interface>::value>
struct InterfaceNameHelper
{
    static QByteArray name()
    { 
        const auto iid = ::qobject_interface_iid<Interface*>();
        assert(iid);
        return iid ? QByteArray::fromRawData(iid, std::strlen(iid)) : QByteArray();
    }
};

template<typename Interface>
struct InterfaceNameHelper<Interface, true>
{
    static QByteArray name()
    { 
        const auto name = Interface::staticMetaObject.className();
        assert(name);
        return QByteArray::fromRawData(name, std::strlen(name));
    }
};

template<typename Key_>
const QVariantList RepoKeysNotFoundException<Key_>::keysGeneric() const
{ 
    using namespace boost::adaptors;
    return qToList(noDetach(_keys) | transformed([] (const Key_& key) {
            return RepoKeyVariantHelper<Key_>::toVariant(key);
        }));
}

template<typename Key_>
const QVariantList RepoKeysAlreadyAddedException<Key_>::keysGeneric() const
{ 
    using namespace boost::adaptors;
    return qToList(noDetach(_keys) | transformed([] (const Key_& key) {
            return RepoKeyVariantHelper<Key_>::toVariant(key);
        }));
}

template<typename Key_>
QVariant RepoLazyInitError<Key_>::keyGeneric() const
{
    return RepoKeyVariantHelper<Key_>::toVariant(_key);
}

}
