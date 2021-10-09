#pragma once

#include <QMutex>
#include <QReadWriteLock>

namespace Addle {
    
namespace aux_util {

template<class Lockable, class Ops>
class LockHandleImpl
{
public:
    LockHandleImpl() = default;
    LockHandleImpl(const LockHandleImpl&) = delete;
    
    explicit LockHandleImpl(Lockable& target)
        : _target(std::addressof(target)), _isLocked(true)
    {
        Ops::lock(_target);
    }
    
    LockHandleImpl(LockHandleImpl&& other)
        : _target(std::exchange(other._target, nullptr)),
        _isLocked(std::exchange(other._isLocked, false))
    {
    }
    
    ~LockHandleImpl()
    {
        assert(_target || !_isLocked);
        if (_target && _isLocked)
        {
            Ops::unlock(_target);
        }
    }
    
    static LockHandleImpl tryLock(Lockable& target, int timeout = 0)
    {
        auto target_ = std::addressof(target);
        return LockHandleImpl(target_, Ops::tryLock(target_, timeout));
    }
    
    static LockHandleImpl makeUnlocked(Lockable& target)
    {
        return LockHandleImpl(std::addressof(target), false);
    }
    
    LockHandleImpl& operator=(const LockHandleImpl&) = delete;
    LockHandleImpl& operator=(LockHandleImpl&& other)
    {
        if (_target && _isLocked)
        {
            Ops::unlock(_target);
        }
        
        _target = std::exchange(other._target, nullptr);
        _isLocked = std::exchange(other._isLocked, false);
        
        return *this;
    }
    
    explicit operator bool() const { return _target && _isLocked; }
    bool operator! () const { return !_target || !_isLocked; }
    
    bool isNull() const { return !_target; }
    
    void clear()
    {
        if (_target && _isLocked)
        {
            Ops::unlock(_target);
        }
        
        _target = nullptr;
        _isLocked = false;
    }
    
    // Refers to the locked status of this handle and not the locked status of
    // the mutex itself.
    bool isLocked() const { return _isLocked; }
    
    void relock()
    {
        assert(_target && !_isLocked);
        
        Ops::lock(_target);
        _isLocked = true;
    }
    
    bool tryRelock(int timeout = 0)
    {
        assert(_target && !_isLocked);
        return (_isLocked = Ops::tryLock(_target, timeout));
    }
    
    void unlock()
    {
        assert(_target && _isLocked);
        Ops::unlock(_target);
        _isLocked = false;
    }
    
private:
    LockHandleImpl(Lockable* lockable, bool isLocked)
        : _target(lockable), _isLocked(isLocked)
    {
    }
    
    Lockable* _target = nullptr;
    bool _isLocked = false;
};
    
template<class Lockable, class Ops>
class RecursiveLockHandleImpl
{
public:
    RecursiveLockHandleImpl() = default;
    RecursiveLockHandleImpl(const RecursiveLockHandleImpl&) = delete;
    
    explicit RecursiveLockHandleImpl(Lockable& target)
        : _target(std::addressof(target)), _lockCount(1)
    {
        Ops::lock(_target);
    }
    
    RecursiveLockHandleImpl(RecursiveLockHandleImpl&& other)
        : _target(std::exchange(other._target, nullptr)),
        _lockCount(std::exchange(other._lockCount, 0))
    {
    }
    
    ~RecursiveLockHandleImpl()
    {
        assert(_target || !_lockCount);
        fullUnlock();
    }
    
    static RecursiveLockHandleImpl tryLock(Lockable& target, int timeout = 0)
    {
        Lockable* target_ = std::addressof(target);
        return RecursiveLockHandleImpl(target_, Ops::tryLock(target_, timeout) ? 1 : 0);
    }
    
    static RecursiveLockHandleImpl makeUnlocked(Lockable& target)
    {
        return RecursiveLockHandleImpl(std::addressof(target), 0);
    }
    
    RecursiveLockHandleImpl& operator=(const RecursiveLockHandleImpl&) = delete;
    RecursiveLockHandleImpl& operator=(RecursiveLockHandleImpl&& other)
    {
        fullUnlock();
        
        _target = std::exchange(other._target, nullptr);
        _lockCount = std::exchange(other._lockCount, false);
        
        return *this;
    }
    
    explicit operator bool() const { return _target && _lockCount; }
    bool operator! () const { return !_target || !_lockCount; }
    
    bool isNull() const { return !_target; }
    
    void clear()
    {
        fullUnlock();
        _target = nullptr;
    }
    
    // Refers to the locked status of this handle and not the locked status of
    // the mutex itself.
    bool isLocked() const { return _lockCount; }
    
    void relock()
    {
        assert(_target);
        
        Ops::lock(_target);
        ++_lockCount;
    }
    
    bool tryRelock(int timeout = 0)
    {
        assert(_target);
        
        if (Ops::tryLock(_target, timeout))
        {
            ++_lockCount;
            return true;
        }
        else
        {
            return false;
        }
    }
    
    void unlock()
    {
        assert(_target && _lockCount);
        Ops::unlock(_target);
        --_lockCount;
    }
    
private:
    RecursiveLockHandleImpl(Lockable* lockable, int lockCount)
        : _target(lockable), _lockCount(lockCount)
    {
    }
    
    void fullUnlock()
    {
        if (_target && _lockCount)
        {
            do { Ops::unlock(_target); } while (--_lockCount);
        }
    }
    
    Lockable* _target = nullptr;
    int _lockCount = 0;
};

struct MutexLockHandleOps
{
    static void lock(QMutex* mutex) noexcept { mutex->lock(); }
    static void unlock(QMutex* mutex) noexcept { mutex->unlock(); }
    static bool tryLock(QMutex* mutex, int timeout) noexcept { return mutex->tryLock(timeout); }
};

struct RecursiveMutexLockHandleOps
{
    static void lock(QRecursiveMutex* mutex) noexcept { mutex->lock(); }
    static void unlock(QRecursiveMutex* mutex) noexcept { mutex->unlock(); }
    static bool tryLock(QRecursiveMutex* mutex, int timeout) noexcept { return mutex->tryLock(timeout); }
};

struct ReadLockHandleOps
{
    static void lock(QReadWriteLock* lock) noexcept { lock->lockForRead(); }
    static void unlock(QReadWriteLock* lock) noexcept { lock->unlock(); }
    static bool tryLock(QReadWriteLock* lock, int timeout) noexcept { return lock->tryLockForRead(timeout); }
};

struct WriteLockHandleOps
{
    static void lock(QReadWriteLock* lock) noexcept { lock->lockForWrite(); }
    static void unlock(QReadWriteLock* lock) noexcept { lock->unlock(); }
    static bool tryLock(QReadWriteLock* lock, int timeout) noexcept { return lock->tryLockForWrite(timeout); }
};

}
 
using MutexLockHandle = aux_util::LockHandleImpl<QMutex, aux_util::MutexLockHandleOps>;
using RecursiveMutexLockHandle = aux_util::RecursiveLockHandleImpl
    <QRecursiveMutex, aux_util::RecursiveMutexLockHandleOps>;

using ReadLockHandle = aux_util::LockHandleImpl<QReadWriteLock, aux_util::ReadLockHandleOps>;
using RecursiveReadLockHandle = aux_util::RecursiveLockHandleImpl
    <QReadWriteLock, aux_util::ReadLockHandleOps>;
    
using WriteLockHandle = aux_util::LockHandleImpl<QReadWriteLock, aux_util::WriteLockHandleOps>;
using RecursiveWriteLockHandle = aux_util::RecursiveLockHandleImpl
    <QReadWriteLock, aux_util::WriteLockHandleOps>;
    
}
