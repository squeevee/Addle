#include <QThreadPool>

#include "asynctask.hpp"

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
            _owner->_isCompleted = false;
            _owner->_isError = false;
            _owner->_error = QSharedPointer<IAddleException>();
        }

        _owner->doTask();

        {
            if (!_owner) return;
            const QMutexLocker lock(&_owner->_stateMutex);
            _owner->_isRunning = false;
            _owner->_isCompleted = true;

            emit _owner->stopped();
            emit _owner->completed();
        }
    }
    catch (IAddleException& ex)
    {
        {
            if (!_owner) return;
            const QMutexLocker lock(&_owner->_stateMutex);
            _owner->_isRunning = false;
            _owner->_isError = true;
            _owner->_error = QSharedPointer<IAddleException>(ex.clone());

            emit _owner->stopped();
            emit _owner->error(_owner->_error);
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