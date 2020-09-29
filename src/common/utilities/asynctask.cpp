/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <cstring> // for strcmp
#include <QTimer>
#include <QMetaObject>
#include <QThreadPool>

#include "asynctask.hpp"

using namespace Addle;

AsyncTask::Worker::Worker(AsyncTask* owner)
    : _owner(owner)
{
}

AsyncTask::Worker::~Worker()
{
    if (!_owner) return;
    const QMutexLocker lock(&_owner->_isRunningMutex);
    _owner->_worker = nullptr;
}

void AsyncTask::Worker::orphan()
{
    if (!_owner) return;
    _owner = nullptr;
}

void AsyncTask::Worker::run()
{
    if (!_owner) return;
    _owner->_isRunningMutex.lock();
    // _owner cannot be deleted.
        
    try
    {
        {
            const QWriteLocker lock(&_owner->_stateLock);
            
            _owner->_elapsedTimer.start();
            
            _owner->_isRunning = true;
            _owner->_hasCompleted = false;
            _owner->_hasFailed = false;
            _owner->_exception = nullptr;
            
            _owner->_progressHandle = std::unique_ptr<TaskProgressHandle>(
                new TaskProgressHandle(std::bind(&AsyncTask::onProgressHandleUpdated, _owner))
            );
        }
        
        _owner->_ioLock.lockForWrite();
        _owner->_ioLock.unlock();
        // No changes can be made to the preconditions now until after the task 
        // has stopped.
        
        _owner->doTask();
        
        {
            const QWriteLocker lock(&_owner->_stateLock);
            
            _owner->_elapsed = _owner->_elapsedTimer.elapsed();
            _owner->_elapsedTimer.invalidate();
            
            
            _owner->_isRunning = false;
            _owner->_hasCompleted = true;
        }
        
        _owner->_progressHandle->complete();
        _owner->_progressHandle = nullptr;
        
        try 
        {
            _owner->onSuccess();
        }
        ADDLE_MISC_CATCH
        
        emit _owner->stopped();
        emit _owner->completed();
    }
    catch (AsyncInterruption& ex)
    {
        {
            const QWriteLocker lock(&_owner->_stateLock);
            
            _owner->_elapsed = _owner->_elapsedTimer.elapsed();
            _owner->_elapsedTimer.invalidate();
            
            _owner->_isRunning = false;
            _owner->_isInterrupted = true;
            _owner->_exception = QSharedPointer<AsyncInterruption>(
                static_cast<AsyncInterruption*>(ex.clone())
            );
        }
        
        _owner->onProgressHandleUpdated();
        _owner->_progressHandle = nullptr;
        
        try 
        {
            _owner->onInterrupted();
        }
        ADDLE_MISC_CATCH
        
        emit _owner->stopped();
        emit _owner->interrupted(ex.code());
    }
    catch (AddleException& ex)
    {
        {
            const QWriteLocker lock(&_owner->_stateLock);
            
            _owner->_elapsed = _owner->_elapsedTimer.elapsed();
            _owner->_elapsedTimer.invalidate();
            
            _owner->_isRunning = false;
            _owner->_hasFailed = true;
            _owner->_exception = QSharedPointer<AddleException>(ex.clone());
        }
        
        _owner->onProgressHandleUpdated();
        _owner->_progressHandle = nullptr;
        
        try 
        {
            _owner->onError();
        }
        ADDLE_MISC_CATCH
        
        emit _owner->stopped();
        emit _owner->failed(_owner->_exception);
    }
    catch(std::exception& ex)
    {
        {
            const QWriteLocker lock(&_owner->_stateLock);
            
            _owner->_elapsed = _owner->_elapsedTimer.elapsed();
            _owner->_elapsedTimer.invalidate();
            
            _owner->_isRunning = false;
            _owner->_hasFailed = true;
            _owner->_exception = QSharedPointer<AddleException>(new UnhandledException(ex));
        }
        
        _owner->onProgressHandleUpdated();
        _owner->_progressHandle = nullptr;
        
        try 
        {
            _owner->onError();
        }
        ADDLE_MISC_CATCH
        
        emit _owner->stopped();
        emit _owner->failed(_owner->_exception);
    }
#ifdef ADDLE_DEBUG
    catch(...)
    {
        {
            const QWriteLocker lock(&_owner->_stateLock);
            
            _owner->_elapsed = _owner->_elapsedTimer.elapsed();
            _owner->_elapsedTimer.invalidate();
            
            _owner->_isRunning = false;
            _owner->_hasFailed = true;
            _owner->_exception = QSharedPointer<AddleException>(new UnhandledException());
        }
        
        _owner->onProgressHandleUpdated();
        _owner->_progressHandle = nullptr;
        
        try 
        {
            _owner->onError();
        }
        ADDLE_MISC_CATCH
        
        emit _owner->stopped();
        emit _owner->failed(_owner->_exception);
    }
#else
    catch(...)
    {
        std::terminate();
    }
#endif

    _owner->_isRunningMutex.unlock();
}

AsyncTask::AsyncTask(QObject* parent)
    : QObject(parent)
{
}

AsyncTask::~AsyncTask()
{
    const QMutexLocker lock(&_isRunningMutex);
    if (_worker) _worker->orphan();
}

void AsyncTask::start()
{
    {
        const QWriteLocker lock(&_stateLock);
        if (_isRunning || _worker) return;
        _worker = new Worker(this);
    }

    QThreadPool::globalInstance()->start(_worker);
}

void AsyncTask::onProgressHandleUpdated()
{   
    ADDLE_ASSERT(_progressHandle);
    
    const QWriteLocker lock(&_stateLock);
    
    qint64 elapsed = _elapsedTimer.isValid() ? _elapsedTimer.elapsed() : _elapsed;
    if (_isRunning && elapsed < _lastProgressElapsed + 1000 / _maxProgressRefreshRate)
        return;
    
    _lastProgressElapsed = elapsed;
    
    {
        double min = _progressHandle->min();
        if (_minProgress != min)
        {
            _minProgress = min;
            emit minProgressChanged(_minProgress);
        }
    }
    
    {
        double max = _progressHandle->max();
        if (_maxProgress != max)
        {
            _maxProgress = max;
            emit maxProgressChanged(_maxProgress);
        }
    }
    
    {
        double progress = _progressHandle->progress();
        if (_progress != progress)
        {
            _progress = progress;
            emit progressChanged(_progress);
        }
    }
    
    if (!_isRunning)
        _progressHandle = nullptr;
}
