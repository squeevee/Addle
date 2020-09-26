/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <cstring> // for strcmp
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

    const QMutexLocker lock(&_owner->_stateMutex);
    _owner->_worker = nullptr;
}

void AsyncTask::Worker::orphan()
{
    if (!_owner) return;
    const QMutexLocker lock(&_owner->_stateMutex);
    _owner = nullptr;
}

void AsyncTask::Worker::run()
{
    if (!_owner) return;
    _owner->_isRunningMutex.lock();
    
    try
    {
        {
            const QMutexLocker lock(&_owner->_stateMutex);
            _owner->_isRunning = true;
            _owner->_hasCompleted = false;
            _owner->_hasFailed = false;
            _owner->_error = QSharedPointer<AddleException>();
        }

        _owner->doTask();
        
        {
            const QMutexLocker lock(&_owner->_stateMutex);
            _owner->_isRunning = false;
            _owner->_hasCompleted = true;

            emit _owner->stopped();
            emit _owner->completed();
        }
    }
    catch (AsyncInterruption& ex)
    {
        const QMutexLocker lock(&_owner->_stateMutex);
        _owner->_isRunning = false;
        
        _owner->_isInterrupted = true;
        _owner->_interruption = QSharedPointer<AsyncInterruption>(
            static_cast<AsyncInterruption*>(ex.clone())
        );
        emit _owner->stopped();
        emit _owner->interrupted(ex.code());
    }
    catch (AddleException& ex)
    {
        const QMutexLocker lock(&_owner->_stateMutex);
        _owner->_isRunning = false;
        
        _owner->_hasFailed = true;
        _owner->_error = QSharedPointer<AddleException>(ex.clone());
        emit _owner->stopped();
        emit _owner->failed(_owner->_error);
    }
    catch(std::exception& ex)
    {
        const QMutexLocker lock(&_owner->_stateMutex);
        _owner->_isRunning = false;
        
        _owner->_hasFailed = true;
        _owner->_error = QSharedPointer<AddleException>(new UnhandledException(ex));
        
        emit _owner->stopped();
        emit _owner->failed(_owner->_error);
    }
#ifdef ADDLE_DEBUG
    catch(...)
    {
        const QMutexLocker lock(&_owner->_stateMutex);
        _owner->_isRunning = false;
        
        _owner->_hasFailed = true;
        _owner->_error = QSharedPointer<AddleException>(new UnhandledException());
        emit _owner->stopped();
        emit _owner->failed(_owner->_error);
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
        const QMutexLocker lock(&_stateMutex);

        if (_isRunning || _worker) return;

        _worker = new Worker(this);
    }

    QThreadPool::globalInstance()->start(_worker);
}
