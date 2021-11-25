#pragma once

#include "compat.hpp"

#include <boost/range/adaptor/transformed.hpp>

#include "exceptions/addleexception.hpp"
#include <exception>

#include <QObject>
#include <QList>
#include <QVariantList>
#include <QMap>

namespace Addle {
namespace aux_config3 {
    

namespace ConfigExceptionEnums {
Q_NAMESPACE_EXPORT(ADDLE_COMMON_EXPORT);

enum InterfaceBindingNotFoundException_Why
{
    NotBound,
    ConditionFailure
};

Q_ENUM_NS(InterfaceBindingNotFoundException_Why)

enum GenericRepoVariantConversionFailure_Which
{
    Key,
    Member
};

Q_ENUM_NS(GenericRepoVariantConversionFailure_Which)

}

struct BindingCondition;
class ModuleConfigCommitFailure : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(ModuleConfigCommitFailure)
public:
    static constexpr bool IsLogicError = true;
    struct ShadowedBinding
    {
        QSharedPointer<BindingCondition> condition;
        const std::type_info* interfaceType;
#ifdef ADDLE_DEBUG
        const std::type_info* implType;
#endif
    };
    
#ifdef ADDLE_DEBUG
    ModuleConfigCommitFailure(QByteArray id, bool idAlreadCommitted, 
        QList<ShadowedBinding> shadowedBindings);
#else
    ModuleConfigCommitFailure(QByteArray id, bool idAlreadCommitted, 
        QList<ShadowedBinding> shadowedBinding)
        : _id(std::move(id)), _idAlreadyCommitted(idAlreadCommitted),
        _shadowedBinding(std::move(shadowedBinding))
    {
    }
#endif
    
    virtual ~ModuleConfigCommitFailure() = default; 
    
    const QByteArray id() const { return _id; }
    bool idAlreadCommitted() const { return _idAlreadyCommitted; }
    const QList<ShadowedBinding> shadowedBindings() const { return _shadowedBindings; }
    
private:
    QByteArray _id;
    bool _idAlreadyCommitted;
    QList<ShadowedBinding> _shadowedBindings;
};

class InterfaceBindingNotFoundException : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(InterfaceBindingNotFoundException);
public:
    static constexpr bool IsLogicError = true;
    
    using Why = ConfigExceptionEnums::InterfaceBindingNotFoundException_Why;
    static constexpr auto NotBound = Why::NotBound;
    static constexpr auto ConditionFailure = Why::ConditionFailure;
    
#ifdef ADDLE_DEBUG
    InterfaceBindingNotFoundException(Why why, const std::type_info& interfaceType,
            QList<QSharedPointer<BindingCondition>> failedConditions = {},
            std::function<QString()> paramsToString = {}
        );
#else
    InterfaceBindingNotFoundException(Why why, const std::type_info& interfaceType)
        : _why(why), _interfaceType(interfaceType)
    {
    }
#endif

    virtual ~InterfaceBindingNotFoundException() = default;
    
    Why why() const { return _why; }
    const std::type_info& interfaceType() const { return _interfaceType; }
    
#ifdef ADDLE_DEBUG
public:
    const QList<QSharedPointer<BindingCondition>> failedConditions() const { return _failedConditions; }
private:
    debug_data_t debugData() const override;
#endif
    
private:
    Why _why;
    const std::type_info& _interfaceType;
#ifdef ADDLE_DEBUG
    QList<QSharedPointer<BindingCondition>> _failedConditions;
    std::function<QString()> _paramsToString;
#endif
};

#ifdef ADDLE_DEBUG

// Some helpers to get debug string forms of several common types for the 
// benefit of exceptions related to repo keys, without requiring debug string 
// utilities in the include tree at this point.
// 
// Key types not accounted for here will fall back on QVariant to provide debug
// strings, but this is somewhat preferred.
// 
// TODO: There's probably a more generally applicable version of this concept,
// but it'd be best to nail down what the include tree should look like before I
// can effectively design that, maybe it's just fine to have the regular debug
// strings here.

template<typename T, bool = std::is_enum_v<T>> 
struct _debugStringHelper
{
    static constexpr bool Supported = false;
    static QString get(...) { Q_UNREACHABLE(); }
};

template<> struct _debugStringHelper<QByteArray>
{
    static constexpr bool Supported = true;
    static QString get(QByteArray);
    static QString get(QList<QByteArray>);
};

template<> struct _debugStringHelper<QString>
{
    static constexpr bool Supported = true;
    static QString get(QString);
    static QString get(QList<QString>);
};

template<> struct _debugStringHelper<std::size_t>
{
    static constexpr bool Supported = true;
    static QString get(std::size_t);
    static QString get(QList<std::size_t>);
};

template<> struct _debugStringHelper<int>
{
    static constexpr bool Supported = true;
    static QString get(int);
    static QString get(QList<int>);
};

template<> struct _debugStringHelper<void*>
{
    static constexpr bool Supported = true;
    static QString get(void*);
    static QString get(QList<void*>);
};

template<typename T> struct _debugStringHelper<T*, false>
{
    using proxy_t = _debugStringHelper<void*>;
    static constexpr bool Supported = true;
    static QString get(T* ptr) { return proxy_t::get(ptr); }
    static QString get(QList<T*> ptrList)
    {
        return proxy_t::get(*reinterpret_cast<QList<void*>*>(&ptrList));
    }
};

template<typename T> struct _debugStringHelper<QSharedPointer<T>, false>
{
    using proxy_t = _debugStringHelper<void*>;
    static constexpr bool Supported = true;
    static QString get(QSharedPointer<T> ptr) { return proxy_t::get(ptr.data()); }
    static QString get(QList<QSharedPointer<T>> ptrList)
    {
        using namespace boost::adaptors;
        return proxy_t::get(qToList(
              noDetach(ptrList) 
            | transformed([] (const QSharedPointer<T> ptr) -> void* { return ptr.data(); })
        ));
    }
};

// extern QString _debugStringHelper_metaEnumImpl(const QMetaObject* mobj, const char* enumName, int index);

template<typename T> struct _debugStringHelper<T, true>
{
    using proxy_t = _debugStringHelper<std::underlying_type_t<T>>;
    static constexpr bool Supported = proxy_t::Supported;
    static QString get(T v) 
    {
        return proxy_t::get((std::underlying_type_t<T>) v);
    }
    static QString get(QList<T> enumList) 
    { 
        return proxy_t::get(*reinterpret_cast<QList<std::underlying_type_t<T>>*>(&enumList)); 
    }
};

#endif

class GenericRepoKeysNotFoundException : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(GenericRepoKeysNotFoundException)
public:
    static constexpr bool IsRuntimeError = true;
    
    GenericRepoKeysNotFoundException();
    virtual ~GenericRepoKeysNotFoundException() = default;
    
    const QVariantList keys() const { return keysGeneric(); }
    
protected:
    virtual const QVariantList keysGeneric() const { return {}; }
};

template<typename Key_>
class RepoKeysNotFoundException final : public GenericRepoKeysNotFoundException
{
    ADDLE_EXCEPTION_BOILERPLATE(RepoKeysNotFoundException)
public:
    explicit RepoKeysNotFoundException(const std::initializer_list<Key_>& keys)
        : _keys(keys)
    {
    }
    
    explicit RepoKeysNotFoundException(QList<Key_> keys = {})
        : _keys(std::move(keys))
    {
    }
    
    virtual ~RepoKeysNotFoundException() = default;
        
    const QList<Key_> keys() const { return _keys; }
    
private:
#ifdef ADDLE_DEBUG
    debug_data_t debugData() const override
    {
        if constexpr (_debugStringHelper<Key_>::Supported)
            return {{ "keys", _debugStringHelper<Key_>::get(_keys) }};
        else
            return GenericRepoKeysNotFoundException::debugData();
    }
#endif
    
    const QVariantList keysGeneric() const override; // defined in aux.hpp
    QList<Key_> _keys;
};

class GenericRepoKeysAlreadyAddedException : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(GenericRepoKeysAlreadyAddedException)
public:
    static constexpr bool IsRuntimeError = true;
    
    GenericRepoKeysAlreadyAddedException();
    virtual ~GenericRepoKeysAlreadyAddedException() = default;
    
    const QVariantList keys() const { return keysGeneric(); }
    
protected:
    virtual const QVariantList keysGeneric() const { return {}; }
};

template<typename Key_>
class RepoKeysAlreadyAddedException final : public GenericRepoKeysAlreadyAddedException
{
public:
    explicit RepoKeysAlreadyAddedException(const std::initializer_list<Key_>& keys)
        : _keys(keys)
    {
    }
    
    explicit RepoKeysAlreadyAddedException(QList<Key_> keys = {}) 
        : _keys(std::move(keys)) 
    {
    }
    
    virtual ~RepoKeysAlreadyAddedException() = default;
    
    RepoKeysAlreadyAddedException(const RepoKeysAlreadyAddedException&) = default;
    RepoKeysAlreadyAddedException(RepoKeysAlreadyAddedException&&) = default;
    
    const QList<Key_> keys() const { return _keys; }
    
private:
#ifdef ADDLE_DEBUG
    debug_data_t debugData() const override
    {
        if constexpr (_debugStringHelper<Key_>::Supported)
            return {{ "keys", _debugStringHelper<Key_>::get(_keys) }};
        else
            return GenericRepoKeysAlreadyAddedException::debugData();
    }
#endif

    const QVariantList keysGeneric() const override; // defined in aux.hpp
    QList<Key_> _keys;
};

class GenericRepoLazyInitError : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(GenericRepoLazyInitError)
public:
    static constexpr bool IsRuntimeError = true;
    
#ifdef ADDLE_DEBUG
    explicit GenericRepoLazyInitError(inner_exception_t inner, bool isStale = false);
#else
    explicit GenericRepoLazyInitError(inner_exception_t inner, bool isStale = false)
        : AddleException({ std::move(inner) }), _isStale(isStale)
    {
    }
#endif

    virtual ~GenericRepoLazyInitError() = default;
    
    QVariant key() const { return keyGeneric(); }
    bool isStale() const { return _isStale; }
    
protected:
    virtual QVariant keyGeneric() const { return {}; };
#ifdef ADDLE_DEBUG
    QString _isStaleProperty_impl() const;
#endif
private:
    bool _isStale;
};

template<typename Key_>
class RepoLazyInitError final : public GenericRepoLazyInitError
{
    ADDLE_EXCEPTION_BOILERPLATE(RepoLazyInitError)
public:
    explicit RepoLazyInitError(Key_ key, inner_exception_t inner, bool isStale = false)
        : GenericRepoLazyInitError(std::move(inner), isStale), _key(std::move(key))
    {
    }

    virtual ~RepoLazyInitError() = default;
    
    Key_ key() const { return _key; }
    
private:
#ifdef ADDLE_DEBUG
    debug_data_t debugData() const override
    {
        if constexpr (_debugStringHelper<Key_>::Supported)
            return {
                { "key", _debugStringHelper<Key_>::get(_key) },
                { "isStale", this->_isStaleProperty_impl() }
            };
        else
            return GenericRepoLazyInitError::debugData();
    }
#endif

    virtual QVariant keyGeneric() const override; // defined in aux.hpp
    
    Key_ _key;
};

// NOTE abstract class
class RepoLogicError : public AddleException 
{
public:
    static constexpr bool IsLogicError = true;
    virtual ~RepoLogicError() = default;
    
protected:
    RepoLogicError(AddleExceptionBuilder b) : AddleException(std::move(b)) {}
};

class RepoLazyInitNotSupportedException final : public RepoLogicError
{
    ADDLE_EXCEPTION_BOILERPLATE(RepoLazyInitNotSupportedException)
public:
#ifdef ADDLE_DEBUG
    RepoLazyInitNotSupportedException();
#else
    RepoLazyInitNotSupportedException() {}
#endif

    virtual ~RepoLazyInitNotSupportedException() = default;
};

class RepoKeyDeductionNotSupportedException final : public RepoLogicError
{
    ADDLE_EXCEPTION_BOILERPLATE(RepoKeyDeductionNotSupportedException)
public:
#ifdef ADDLE_DEBUG
    RepoKeyDeductionNotSupportedException();
#else
    RepoKeyDeductionNotSupportedException() {}
#endif

    virtual ~RepoKeyDeductionNotSupportedException() = default;
};

class GenericRepoVariantConversionFailure final : public RepoLogicError
{
    ADDLE_EXCEPTION_BOILERPLATE(GenericRepoVariantConversionFailure)
public:
    using Which = ConfigExceptionEnums::GenericRepoVariantConversionFailure_Which;
    static constexpr auto Key       = Which::Key;
    static constexpr auto Member    = Which::Member;
    
#ifdef ADDLE_DEBUG
    GenericRepoVariantConversionFailure(Which which, QVariant original = {}, int expectedType = 0);
#else
    GenericRepoVariantConversionFailure(Which which, QVariant original = {}, int expectedType = 0))
        : _which(which), _original(std::move(original)), _expectedType(expectedType)
    {
    }
#endif

    virtual ~GenericRepoVariantConversionFailure() = default;
    
    Which which() const { return _which; }
    QVariant original() const { return _original; }
    int expectedType() const { return _expectedType; }
    
private:
#ifdef ADDLE_DEBUG
    static QString _expectedType_impl(const AddleException& ex_);
#endif
    
    Which _which;
    QVariant _original;
    int _expectedType;
};

}

using aux_config3::GenericRepoKeysNotFoundException;
using aux_config3::RepoKeysNotFoundException;
using aux_config3::GenericRepoKeysAlreadyAddedException;
using aux_config3::RepoKeysAlreadyAddedException;
using aux_config3::RepoLogicError;
using aux_config3::RepoLazyInitNotSupportedException;
using aux_config3::RepoKeyDeductionNotSupportedException;
using aux_config3::GenericRepoVariantConversionFailure;
using aux_config3::GenericRepoLazyInitError;
using aux_config3::RepoLazyInitError;

}
