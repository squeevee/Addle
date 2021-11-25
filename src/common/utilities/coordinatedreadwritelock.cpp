#include <QThread>
#include "./coordinatedreadwritelock.hpp"

using namespace Addle;

CoordinatedReadWriteLock::~CoordinatedReadWriteLock()
{
    for (auto&& state : _states)
    {
        assert(!state.second->count);
        delete state.second;
    }
}

bool CoordinatedReadWriteLock::isLocked() const
{
    auto state = getCurrentThreadState();
    return state && state->count;
}

bool CoordinatedReadWriteLock::isLockedForRead() const
{
    auto state = getCurrentThreadState();
    return state && state->kind == LockedForRead;
}

bool CoordinatedReadWriteLock::isLockedForWrite() const
{
    auto state = getCurrentThreadState();
    return state && state->kind == LockedForWrite;
}

int CoordinatedReadWriteLock::onUnlock(std::function<void ()> callback)
{
    auto& state = getCurrentThreadState();
    
    if (!state.count)
        return -1;
    
    int id = 0;
    if (!state.onUnlock.empty()) 
        id = state.onUnlock.back().first + 1;
    
    state.onUnlock.push_back({ id, std::move(callback) });
    return id;
}

void CoordinatedReadWriteLock::removeOnUnlock(int id)
{
    auto& state = getCurrentThreadState();
    
    auto find = std::lower_bound(
            state.onUnlock.cbegin(), state.onUnlock.cend(), id,
            [](const auto& p, int id_) { return p.first < id_; }
        );
    
    assert(find != state.onUnlock.cend() && (*find).first == id && "Id not found");
    state.onUnlock.erase(find);
}

CoordinatedReadWriteLock::ThreadLockState& 
CoordinatedReadWriteLock::getCurrentThreadState() noexcept
{
    ThreadLockState* result;
    
    const QMutexLocker stateLock(&_stateMutex);
    auto threadId = QThread::currentThreadId();
    auto find = _states.find(threadId);
    
    if (find != _states.end())
        result = (*find).second;
    else
        _states.emplace(threadId, (result = new ThreadLockState));
    assert(result);
    
    return *result;
}

const CoordinatedReadWriteLock::ThreadLockState*
CoordinatedReadWriteLock::getCurrentThreadState() const noexcept
{
    const QMutexLocker stateLock(&_stateMutex);
    auto threadId = QThread::currentThreadId();
    auto find = _states.find(threadId);
    
    return (find != _states.end()) ? (*find).second : nullptr;
}

void CoordinatedReadWriteLock::lockForRead_p() noexcept
{
    auto& state = getCurrentThreadState();
    
    assert(state.kind != LockedForWrite);
    _lock.lockForRead();
    
    state.kind = LockedForRead;
    ++(state.count);
}

bool CoordinatedReadWriteLock::tryLockForRead_p(int timeout) noexcept
{
    auto& state = getCurrentThreadState();
    
    if (state.kind != LockedForWrite && _lock.tryLockForRead(timeout))
    {
        state.kind = LockedForRead;
        ++(state.count);
        return true;
    }
    
    return false;
}

void CoordinatedReadWriteLock::lockForWrite_p() noexcept
{
    auto& state = getCurrentThreadState();
    
    assert(state.kind != LockedForRead);
    state.kind = LockedForWrite;
    ++(state.count);
    
    _lock.lockForWrite();
}

bool CoordinatedReadWriteLock::tryLockForWrite_p(int timeout) noexcept
{
    auto& state = getCurrentThreadState();
    
    if (state.kind != LockedForRead && _lock.tryLockForWrite(timeout))
    {
        state.kind = LockedForWrite;
        ++(state.count);
        return true;
    }
    
    return false;
}

void CoordinatedReadWriteLock::lockAuto_p() noexcept
{
    auto& state = getCurrentThreadState();
    
    if (state.kind == LockedForWrite)
    {
        _lock.lockForWrite();
        ++(state.count);
    }
    else
    {
        _lock.lockForRead();
        state.kind = LockedForRead;
        ++(state.count);
    }
}

bool CoordinatedReadWriteLock::tryLockAuto_p(int timeout) noexcept
{
    auto& state = getCurrentThreadState();
    
    if (state.kind == LockedForWrite)
    {
        if (_lock.tryLockForWrite(timeout))
        {
            ++(state.count);
            return true;
        }
    }
    else
    {
        if (_lock.tryLockForRead(timeout))
        {
            state.kind = LockedForRead;
            ++(state.count);
            return true;
        }
    }
    
    return false;
}

void CoordinatedReadWriteLock::unlock_p() noexcept
{
    _lock.unlock();
    std::vector< std::pair<int, std::function<void()>> > callbacks;
    
    {
        const QMutexLocker stateLock(&_stateMutex);
        auto find = _states.find(QThread::currentThreadId());
        assert(find != _states.end() && (*find).second);
        if (!(--((*find).second->count)))
        {
            callbacks = std::move((*find).second->onUnlock);
            delete (*find).second;
            _states.erase(find);
        }
    }
    
    for (auto&& c : callbacks) c.second();
}
