#pragma once

#include <unordered_map>
#include <functional>
#include <vector>

#include <QMutex>
#include <QtGlobal>

#include "./ranges.hpp"
#include "./lockhandle.hpp"

namespace Addle {

/**
 * Wrapper for a recursive QReadWriteLock supporting a special lock operation
 * called "lockAuto" -- if the QReadWriteLock is not locked, or is locked for 
 * reading on the current thread, then lockAuto locks it for reading. Iff the 
 * QReadWriteLock is locked for writing, then lockAuto locks it for writing.
 * 
 * A thread-safe interface may expose coordinated lock handles to clients, to 
 * allow multiple operations to be conducted within the same lock. If the 
 * implementation uses lockAuto to guard read-only operations (instead of 
 * lockForRead), then those same read-only operations may be done during while a
 * write lock is held, without needing to expose redundant lockForRead and 
 * lockForWrite versions of read-only functions, and without making it the 
 * client's responsibility to externally manage the interface's locks.
 */
class CoordinatedReadWriteLock
{
    struct ReadOps
    {
        static void lock(CoordinatedReadWriteLock* lock) noexcept { lock->lockForRead_p(); }
        static void unlock(CoordinatedReadWriteLock* lock) noexcept { lock->unlock_p(); }
        static bool tryLock(CoordinatedReadWriteLock* lock, int timeout) noexcept { return lock->tryLockForRead_p(timeout); }
    };
    
    struct WriteOps
    {
        static void lock(CoordinatedReadWriteLock* lock) noexcept { lock->lockForWrite_p(); }
        static void unlock(CoordinatedReadWriteLock* lock) noexcept { lock->unlock_p(); }
        static bool tryLock(CoordinatedReadWriteLock* lock, int timeout) noexcept { return lock->tryLockForWrite_p(timeout); }
    };
    
    struct AutoOps
    {
        static void lock(CoordinatedReadWriteLock* lock) noexcept { lock->lockAuto_p(); }
        static void unlock(CoordinatedReadWriteLock* lock) noexcept { lock->unlock_p(); }
        static bool tryLock(CoordinatedReadWriteLock* lock, int timeout) noexcept { return lock->tryLockAuto_p(timeout); }
    };
    
public:
    using ReadHandle  = aux_util::RecursiveLockHandleImpl<CoordinatedReadWriteLock, ReadOps>;
    using WriteHandle = aux_util::RecursiveLockHandleImpl<CoordinatedReadWriteLock, WriteOps>;
    using AutoHandle  = aux_util::RecursiveLockHandleImpl<CoordinatedReadWriteLock, AutoOps>;
    
    // TODO: convert read/write to auto (and back?)
    
    CoordinatedReadWriteLock() : _lock(QReadWriteLock::Recursive) {}
    ~CoordinatedReadWriteLock();

    CoordinatedReadWriteLock(const CoordinatedReadWriteLock&) = delete;
    CoordinatedReadWriteLock(CoordinatedReadWriteLock&&) = delete;
    
    ReadHandle lockForRead() noexcept { return ReadHandle(*this); }
    ReadHandle tryLockForRead(int timeout = 0) noexcept
    { return ReadHandle::tryLock(*this, timeout); }
    
    WriteHandle lockForWrite() noexcept { return WriteHandle(*this); }
    WriteHandle tryLockForWrite(int timeout = 0) noexcept
    { return WriteHandle::tryLock(*this, timeout); }
    
    AutoHandle lockAuto() noexcept { return AutoHandle(*this); }
    AutoHandle tryLockAuto(int timeout = 0) noexcept 
    { return AutoHandle::tryLock(*this, timeout); }
    
    bool isLocked() const;
    bool isLockedForRead() const;
    bool isLockedForWrite() const;
    
    // Registers a function object to be called when this thread releases this 
    // lock, and returns an integer id that can be used to remove the function.
    // If this thread is not currently locked, this does nothing and returns -1.
    int onUnlock(std::function<void()> callback);
    void removeOnUnlock(int id);
    
private:
    enum LockKind
    {
        Unlocked = 0,
        LockedForRead,
        LockedForWrite
    };
    
    struct ThreadLockState
    {
        ThreadLockState() = default;
        ThreadLockState(const ThreadLockState&) = delete;
        ThreadLockState(ThreadLockState&&) = delete;
        
        LockKind kind = Unlocked;
        unsigned count = 0;
        
        std::vector< std::pair<int, std::function<void()>> > onUnlock;
    };
    
    ThreadLockState& getCurrentThreadState() noexcept;
    const ThreadLockState* getCurrentThreadState() const noexcept;
    
    void lockForRead_p() noexcept;
    bool tryLockForRead_p(int timeout) noexcept;
    
    void lockForWrite_p() noexcept;
    bool tryLockForWrite_p(int timeout) noexcept;
    
    void lockAuto_p() noexcept;
    bool tryLockAuto_p(int timeout) noexcept;
    
    void unlock_p() noexcept;
    
    QReadWriteLock _lock;
    
    // to prevent deadlocks, _stateMutex must never be locked when _lock is 
    // acquired or released.
    mutable QMutex _stateMutex;
    
    // A LockState instance must only be accessed or deleted by the thread that
    // it corresponds to. That's why _states only holds a pointer, so no matter 
    // what's happening with this structure in another thread, the instances are
    // untouched and thus do not require guarding
    std::unordered_map<Qt::HANDLE, ThreadLockState*> _states;
};
    
}
