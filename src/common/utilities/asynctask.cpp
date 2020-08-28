/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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
    try
    {
        {
            if (!_owner) return;
            const QMutexLocker lock(&_owner->_stateMutex);
            _owner->_isRunning = true;
            _owner->_hasCompleted = false;
            _owner->_hasFailed = false;
            _owner->_error = QSharedPointer<AddleException>();
        }

        _owner->doTask();

        {
            if (!_owner) return;
            const QMutexLocker lock(&_owner->_stateMutex);
            _owner->_isRunning = false;
            _owner->_hasCompleted = true;

            emit _owner->stopped();
            emit _owner->completed();
        }
    }
    catch (AddleException& ex)
    {
        {
            if (!_owner) return;
            const QMutexLocker lock(&_owner->_stateMutex);
            _owner->_isRunning = false;
            _owner->_hasFailed = true;
            _owner->_error = QSharedPointer<AddleException>(ex.clone());

            emit _owner->stopped();
            emit _owner->failed(_owner->_error);
        }
    }
    // catch(...) ?
}

AsyncTask::AsyncTask(QObject* parent)
    : QObject(parent)
{
}

AsyncTask::~AsyncTask()
{
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