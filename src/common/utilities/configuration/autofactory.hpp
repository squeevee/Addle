/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef AUTOFACTORY_HPP
#define AUTOFACTORY_HPP

#include <type_traits>

#include "servicelocator.hpp"

#include "interfaces/traits.hpp"
#include "utilities/errors.hpp"

#include "interfaces/config/ifactory.hpp"

namespace Addle {
    
template<class Impl_>
static inline typename std::enable_if<
    std::is_base_of<QObject, typename std::remove_cv<Impl_>::type>::value,
    QByteArray
>::type _implNameHelper()
{
    return QByteArray(Impl_::staticMetaObject.className());
}

template<class Impl_>
static inline typename std::enable_if<
    !std::is_base_of<QObject, typename std::remove_cv<Impl_>::type>::value,
    QByteArray
>::type _implNameHelper()
{
    return QStringLiteral("`%1`")
        .arg(typeid(Impl_).name())
        .toUtf8();
}

template<class Interface_, class Impl_>
static inline QByteArray _factoryNameHelper(const QString& baseName)
{
    return QStringLiteral("Addle::%1<`%2`, %3>")
        .arg(baseName)
        .arg(typeid(Interface_).name())
        .arg(_implNameHelper<Impl_>().constData())
        .toUtf8();
}


/**
 * A generic template-based object factory.
 */
template<class Interface, class Impl>
class AutoFactory : public IFactory
{
    static_assert(
        Traits::is_makeable<Interface>::value,
        "Interface must be makeable"
    );

public:
    virtual ~AutoFactory() = default;

    virtual void* make(AddleId id = AddleId()) const 
    {
        //ADDLE_ASSERT(!id);
        return reinterpret_cast<void*>(
            static_cast<Interface*>(new Impl)
        );
    }

    void delete_(void* obj) const 
    {
        delete reinterpret_cast<Interface*>(obj);
    }
    
#ifdef ADDLE_DEBUG
    const char* factoryName() const override
    {
        static QByteArray name = _factoryNameHelper<Interface, Impl>(
            QStringLiteral("AutoFactory")
        );
        return name.constData();
    }
    const char* implementationName() const override
    {
        static QByteArray name = _implNameHelper<Impl>();
        return name.constData();
    }
#endif
};

template<class Interface, class Impl>
class AutoFactoryById : public IFactory
{
    static_assert(
        Traits::is_makeable<Interface>::value,
        "Interface must be makeable"
    );
    
public:
    virtual ~AutoFactoryById() = default;

    virtual void* make(AddleId id = AddleId()) const 
    {
        return reinterpret_cast<void*>(
            static_cast<Interface*>(new Impl(id))
        );
    }

    void delete_(void* obj) const 
    {
        delete reinterpret_cast<Interface*>(obj);
    }
    
#ifdef ADDLE_DEBUG
    const char* factoryName() const override
    {
        static QByteArray name = _factoryNameHelper<Interface, Impl>(
            QStringLiteral("AutoFactoryById")
        );
        return name.constData();
    }
    const char* implementationName() const override
    {
        static QByteArray name = _implNameHelper<Impl>();
        return name.constData();
    }
#endif
};

} // namespace Addle

#endif // AUTOFACTORY_HPP
