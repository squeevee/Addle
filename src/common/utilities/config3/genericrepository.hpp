#pragma once

#include "./aux.hpp"
#include "./configexceptions.hpp"

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
#include <QtDebug>
#endif

namespace Addle {
namespace aux_config3 {

struct GenericRepoMemberCore
{
    QVariant member;
    void* rawMember = nullptr;
    AddleException::inner_exception_t initException;
};

}
    
class GenericRepoMemberRef
{
public:
    GenericRepoMemberRef() = default;
    
    GenericRepoMemberRef(const GenericRepoMemberRef& other)
        : _initialized(other._initialized)
    {
        if (_initialized) other.handle()->clone(&_buffer);
    }
    
    ~GenericRepoMemberRef() noexcept
    {
        if (_initialized) handle()->~IHandle();
    }
    
    GenericRepoMemberRef& operator=(const GenericRepoMemberRef& other)
    {
        if (_initialized) handle()->~IHandle();
        
        _initialized = other._initialized;
        if (_initialized) other.handle()->clone(&_buffer);
        
        return *this;
    }
    
    explicit operator bool () const { return _initialized; }
    bool operator!() const { return !_initialized; }
    bool isNull() const { return !_initialized; }
    
    bool operator==(const GenericRepoMemberRef& other) const
    {
        // Return true if:
        // - this and other are both uninitialized
        // - this and other are both initialized, and this handle returns true 
        //   when calling `equal` with other's handle
        return Q_UNLIKELY(this == &other)
            || Q_UNLIKELY(!_initialized && !other._initialized)
            || (
                _initialized && other._initialized
                && handle()->equal(other.handle())
            );
    }
    
    bool operator!=(const GenericRepoMemberRef& other) const
    {
        return !(*this == other);
    }
    
    Q_DECL_PURE_FUNCTION
    QMetaType::Type keyType() const noexcept
    { return _initialized ? handle()->keyType() : QMetaType::UnknownType; }
    
    Q_DECL_PURE_FUNCTION
    QByteArray interfaceName() const noexcept
    { return _initialized ? handle()->interfaceName() : QByteArray(); }
    
    Q_DECL_PURE_FUNCTION
    bool supportsLazyInitialization() const noexcept
    { return _initialized && handle()->supportsLazyInitialization(); }
    
    Q_DECL_PURE_FUNCTION
    bool isReadOnly() const noexcept
    { return _initialized && handle()->isReadOnly(); }
    
    QObject* repository() const noexcept
    { return _initialized ? handle()->repository() : nullptr; }
    
    QVariant key() const { return _initialized ? handle()->key() : QVariant(); }
    
    template<class Interface>
    QSharedPointer<Interface> member() const
    {
        assert(_initialized);
        return aux_config3::RepoInterfaceVariantHelper<std::remove_const_t<Interface>>::fromVariant(handle()->getMember());
    }
    
    template<class Interface>
    QSharedPointer<Interface> tryMember() const noexcept
    {
        if (!_initialized) return nullptr;
        return aux_config3::RepoInterfaceVariantHelper<std::remove_const_t<Interface>>::fromVariant(handle()->getMember(true), true);
    }
    
    template<class Interface>
    void setMember(QSharedPointer<Interface> member)
    {
        static_assert(!std::is_const_v<Interface>);
        assert(_initialized
#ifdef ADDLE_DEBUG
                && !handle()->isReadOnly()
#endif
            );
        handle()->setMember(aux_config3::RepoInterfaceVariantHelper<Interface>::toVariant(std::move(member)));
    }
    
    template<class Interface>
    GenericRepoMemberRef& operator=(QSharedPointer<Interface> member) { setMember(std::move(member)); return *this; }
    
    template<class Interface>
    Interface* rawMember() const
    {
        assert(_initialized);
        return static_cast<Interface*>(handle()->getRawMember());
    }
    
    template<class Interface>
    Interface* tryRawMember() const noexcept
    {
        return _initialized ? static_cast<Interface*>(handle()->getRawMember(true)) : nullptr;
    }
    
    bool memberIsInitialized() const noexcept
    {
        return _initialized && handle()->memberIsInitialized();
    }
    
    bool memberIsError() const noexcept
    {
        return _initialized && handle()->memberIsError();
    }
    
    AddleException::inner_exception_t initException() const noexcept
    {
        return _initialized ? handle()->getInitException() : AddleException::inner_exception_t();
    }
    
    void releaseLock()
    { if (_initialized) handle()->releaseLock(); }
    
protected:
    static constexpr std::size_t BUFFER_SIZE = 64 - sizeof(bool);
    // the size is basically arbitrary but has to be big enough to hold an 
    // instance of any IHandle implementation (no logic is available for using 
    // the heap even as a backup).
    
    class IHandle
    {
    public:
        virtual ~IHandle() noexcept = default;
            
        Q_DECL_PURE_FUNCTION
        virtual QMetaType::Type keyType() const noexcept = 0;
        
        Q_DECL_PURE_FUNCTION
        virtual QByteArray interfaceName() const noexcept = 0;
        
        Q_DECL_PURE_FUNCTION
        virtual bool supportsLazyInitialization() const noexcept = 0;
        
        Q_DECL_PURE_FUNCTION
        virtual bool isReadOnly() const noexcept = 0;
        
        virtual QObject* repository() const noexcept = 0;
        
        virtual void clone(void* dest) const noexcept = 0;
        virtual bool equal(const IHandle* other) const noexcept = 0;
        
        virtual void increment() noexcept = 0;
        virtual void decrement() noexcept = 0;
        
        virtual void releaseLock() noexcept = 0;
        
        virtual QVariant key() const noexcept = 0;
        
        virtual bool memberIsInitialized() const noexcept = 0;
        virtual bool memberIsError() const noexcept = 0;
        
        virtual aux_config3::GenericRepoMemberCore getMemberCore() const noexcept = 0;
        
        virtual QVariant getMember(bool silenceErrors = false) const = 0;
        virtual void* getRawMember(bool silenceErrors = false) const = 0;
        virtual void setMember(QVariant member) = 0;
        virtual AddleException::inner_exception_t getInitException() const noexcept = 0;
    };
    
    alignas(std::max_align_t) char _buffer[BUFFER_SIZE];
    bool _initialized = false;
    
    IHandle* handle() { return _initialized ? reinterpret_cast<IHandle*>(&_buffer) : nullptr; }
    const IHandle* handle() const { return _initialized ? reinterpret_cast<const IHandle*>(&_buffer) : nullptr; }
    
    friend class GenericRepoMember;
};

class GenericRepoMember
{
public:
    GenericRepoMember() = default;
    GenericRepoMember(const GenericRepoMember&) = default;
    GenericRepoMember(const GenericRepoMemberRef& ref)
        : _key(ref.key())
        , _interfaceName(ref._initialized ? ref.handle()->interfaceName() : QByteArray())
        , _core(ref._initialized ? ref.handle()->getMemberCore() : aux_config3::GenericRepoMemberCore())
    {
    }
    
    template<typename Interface>
    GenericRepoMember(QVariant key, QSharedPointer<Interface> member)
        : _key(key)
        , _interfaceName(aux_config3::InterfaceNameHelper<Interface>::name())
        , _core { 
              aux_config3::RepoInterfaceVariantHelper<Interface>::toVariant(member) 
            , member.data()
          }
    {
        static_assert(!std::is_const_v<Interface>);
    }
    
    GenericRepoMember(QVariant key, AddleException::inner_exception_t initException)
        : _key(key)
        , _core {
              QVariant()    // (member)
            , nullptr       // (rawMember)
            , std::move(initException)
          }
    {
    }
    
    bool isNull() const { return _key.isNull(); }
    bool isEmpty() const { return isNull() && _core.member.isNull() && !_core.rawMember; }
    
    QVariant key() const { return _key; }
    
    QByteArray interfaceName() const { return _interfaceName; }
    
    bool memberIsInitialized() const { return !isNull() && _core.rawMember; }
    bool memberIsError() const 
    { 
        return !isNull() && !_core.rawMember 
            && std::visit([](const auto& v) { return (bool) v; }, _core.initException); 
    }
    
    template<typename Interface>
    QSharedPointer<Interface> member() const
    {
        assert(!isEmpty());
        return aux_config3::RepoInterfaceVariantHelper<std::remove_cv_t<Interface>>::fromVariant(_core.member);
    }
    
    template<typename Interface>
    QSharedPointer<Interface> tryMember() const noexcept
    {
        if (isEmpty()) return nullptr;
        return aux_config3::RepoInterfaceVariantHelper<std::remove_cv_t<Interface>>::fromVariant(_core.member, true);
    }
    
    template<typename Interface>
    Interface* rawMember() const noexcept
    {
        return static_cast<Interface*>(_core.rawMember);
    }
    
    AddleException::inner_exception_t initException() const { return _core.initException; }
    
private:
    QVariant _key;
    QByteArray _interfaceName;
    aux_config3::GenericRepoMemberCore _core;
};

class GenericRepository : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMetaType::Type keyType READ keyType CONSTANT)
    Q_PROPERTY(QByteArray interfaceName READ interfaceName CONSTANT)
    Q_PROPERTY(bool supportsLazyInitialization READ supportsLazyInitialization CONSTANT)
    Q_PROPERTY(bool supportsKeyDeduction READ supportsKeyDeduction CONSTANT)
    Q_PROPERTY(QVariantList keys READ keys NOTIFY keysChanged)
    
public:
    class iterator : public boost::iterator_facade<
            iterator, const GenericRepoMemberRef, std::bidirectional_iterator_tag
        >,
        private GenericRepoMemberRef
    {
    public:
        iterator() = default;
        iterator(const iterator&) = default;
        
        using GenericRepoMemberRef::isReadOnly;
        using GenericRepoMemberRef::releaseLock;
        
    private:
        iterator(const GenericRepoMemberRef& ref)
            : GenericRepoMemberRef(ref)
        {
        }
        
        const GenericRepoMemberRef& dereference() const
        {
            return *reinterpret_cast<const GenericRepoMemberRef*>(this);
        }
        
        void increment() { assert(this->handle()); this->handle()->increment(); }
        void decrement() { assert(this->handle()); this->handle()->decrement(); }
        
        friend class GenericRepository;
        friend class boost::iterator_core_access;
    };
    
    GenericRepository() = default;
    virtual ~GenericRepository() = default;
    
    Q_DECL_PURE_FUNCTION
    virtual QMetaType::Type keyType() const noexcept = 0;
    
    Q_DECL_PURE_FUNCTION
    virtual QByteArray interfaceName() const noexcept = 0;
    
    Q_DECL_PURE_FUNCTION
    virtual bool supportsLazyInitialization() const noexcept = 0;
    
    Q_DECL_PURE_FUNCTION
    virtual bool supportsKeyDeduction() const noexcept = 0;
    
    Q_INVOKABLE virtual bool empty() const noexcept = 0;
    Q_INVOKABLE virtual std::size_t size() const noexcept = 0;
    
#ifdef ADDLE_DEBUG
    iterator begin() noexcept 
    { 
        auto result = iterator(beginGeneric()); 
        assert(!result.isReadOnly());
        return result;
    }
    
    iterator end() noexcept 
    { 
        auto result = iterator(endGeneric()); 
        assert(!result.isReadOnly());
        return result;
    }
    
    iterator begin() const noexcept 
    { 
        auto result = iterator(beginGeneric()); 
        assert(result.isReadOnly());
        return result;
    }
    
    iterator end() const noexcept
    { 
        auto result = iterator(endGeneric()); 
        assert(result.isReadOnly());
        return result;
    }
#else
    iterator begin() noexcept { return iterator(beginGeneric()); }
    iterator end() noexcept { return iterator(endGeneric()); }
    
    iterator begin() const noexcept { return iterator(beginGeneric()); }
    iterator end() const noexcept { return iterator(endGeneric()); }
#endif
    
    QVariantList keys() const noexcept
    {
        QMutexLocker lock(&_genericMembersCacheMutex);
        
        auto result = _genericKeysCache;
        if (result.isEmpty())
        {
            {
                for (const auto& ref : *this)
                    result.append(ref.key());
            }
            
            if (QMetaType::hasRegisteredComparators(keyType()))
            {
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
                std::sort(result.begin(), result.end());
QT_WARNING_POP
            }
            
            _genericKeysCache = result;
        }
        
        return result;
    }
    
    Q_INVOKABLE QList<GenericRepoMember> members() const noexcept 
    {
        const QMutexLocker lock(&_genericMembersCacheMutex);
        auto result = _genericMembersCache;
        if (result.isEmpty())
        {
            {
                for (const auto& ref : *this)
                    result.append(GenericRepoMember(ref));
            }
            
            if (QMetaType::hasRegisteredComparators(keyType()))
            {
                std::sort(result.begin(), result.end(),
                    [](const GenericRepoMember& lhs, const GenericRepoMember& rhs) {
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
                        return lhs.key() < rhs.key();
QT_WARNING_POP
                    });
            }
            
            _genericMembersCache = result;
        }
        
        return result;
    }
    
    Q_INVOKABLE QList<GenericRepoMember> initializedMembers() const noexcept
    {
        if (supportsLazyInitialization())
        {
            QList<GenericRepoMember> result;
            for (const auto& ref : *this)
            {
                if (ref.memberIsInitialized())
                    result += GenericRepoMember(ref);
            }
            return result;
        }
        else
        {
            return members();
        }
    }
    
    Q_INVOKABLE virtual bool contains(QVariant key) const noexcept = 0;
    
    Q_INVOKABLE GenericRepoMember get(QVariant key) const noexcept
    {
        return GenericRepoMember(at(key));
    }
    
    virtual GenericRepoMemberRef at(QVariant key) noexcept = 0;
    virtual GenericRepoMemberRef at(QVariant key) const noexcept = 0;
    
    inline GenericRepoMemberRef operator[](QVariant key) noexcept { return at(std::move(key)); }
    inline GenericRepoMemberRef operator[](QVariant key) const noexcept { return at(std::move(key)); }
    
    void addKey(QVariant key) { addKeys({ key }); }
    virtual void addKeys(QVariantList keys) = 0;
    
    void addMember(Addle::GenericRepoMember member) { addMembers({ member }); }
    virtual void addMembers(QList<Addle::GenericRepoMember> members) = 0;
    
//     void removeKey(QVariant key) { removeKeys(QVariantList({ key })); }
//     virtual void removeKeys(QVariantList keys) = 0;
    
    virtual CoordinatedReadWriteLock::ReadHandle lockForRead(int timeout = -1) const = 0;
    virtual CoordinatedReadWriteLock::WriteHandle lockForWrite(int timeout = -1) const = 0;
    
    template<typename Interface>
    static QVariant memberToVariant(QSharedPointer<Interface> member)
    {
        return aux_config3::RepoInterfaceVariantHelper<Interface>::toVariant(member);
    }
    
    template<typename Key, typename Interface>
    static QVariant keyMemberPairToVariant(Key&& key, QSharedPointer<Interface> member)
    {
        return aux_config3::RepoInterfaceVariantHelper<Interface>::pairToVariant(
                std::forward<Key>(key),
                std::move(member)
            );
    }
    
public slots:
    bool tryAddKey(QVariant key) noexcept { return tryAddKeys({ key }); }
    virtual bool tryAddKeys(QVariantList keys) noexcept = 0;
    
    bool tryAddMember(Addle::GenericRepoMember member) noexcept { return tryAddMembers({ member }); }
    virtual bool tryAddMembers(QList<Addle::GenericRepoMember> members) noexcept = 0;
    
//     bool tryRemoveKey(QVariant key) noexcept { return tryRemoveKeys({ key }); }
//     virtual bool tryRemoveKeys(QVariantList keys) noexcept = 0;
    
signals:
    void keysChanged(QVariantList keys);
    void keysAdded(QVariantList keys);
    void keysRemoved(QVariantList keys);

    void membersAdded(QList<Addle::GenericRepoMember> newMembers);
    void memberErrors(QList<Addle::GenericRepoMember> errors);
    
protected:
    virtual void initAllMembers() const noexcept {}
    
    virtual GenericRepoMemberRef beginGeneric() noexcept = 0;
    virtual GenericRepoMemberRef endGeneric() noexcept = 0;
    virtual GenericRepoMemberRef beginGeneric() const noexcept = 0;
    virtual GenericRepoMemberRef endGeneric() const noexcept = 0;
    
    mutable QVariantList _genericKeysCache;
    mutable QList<GenericRepoMember> _genericMembersCache;
    mutable QMutex _genericMembersCacheMutex;
};

}

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
inline QDebug operator<<(QDebug debug, const Addle::GenericRepoMemberRef& ref)
{
    if (ref.isNull())
    {
        debug << "GenericRepoMemberRef(null)";
    }
    else
    {
        debug << "GenericRepoMemberRef(";
        
        auto key = ref.key();
        if (!key.isNull())
            debug << "key:" << key;
        
        auto interfaceName = ref.interfaceName();
        if (!interfaceName.isEmpty())
            debug << "interface:" << interfaceName;
        
        void* member;
        if (!ref.memberIsInitialized() && !ref.memberIsError())
        {
            debug << "(uninitialized member)";
        }
        else if (ref.memberIsInitialized() && (member = ref.tryRawMember<void>()))
        {
            debug << "member:" << member;
        }
        else if (ref.memberIsError())
        {
            debug << "(error member)";
        }
    }
    return debug << ")";
}

inline QDebug operator<<(QDebug debug, const Addle::GenericRepoMember& member)
{
    if (member.isNull())
    {
        debug << "GenericRepoMember(null)";
    }
    else
    {
        debug << "GenericRepoMember(";
        
        if (!member.key().isNull())
            debug << "key:" << member.key();
        
        if (!member.interfaceName().isEmpty())
            debug << "interface:" << member.interfaceName();
        
        if (!member.memberIsInitialized() && !member.memberIsError())
        {
            debug << "(uninitialized member)";
        }
        else if (member.memberIsInitialized() && member.rawMember<void>())
        {
            debug << "member:" << member.rawMember<void>();
        }
        else if (member.memberIsError())
        {
            debug << "(error member)";
        }
    }
    return debug << ")";
}
#endif

Q_DECLARE_METATYPE(Addle::GenericRepoMember);
