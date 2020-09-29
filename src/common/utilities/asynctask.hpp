/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ASYNCTASK_HPP
#define ASYNCTASK_HPP

#include "compat.hpp"
#include "exceptions/addleexception.hpp"
#include "utilities/errors.hpp"
#include "utilities/taskprogresshandle.hpp"

#include <QSharedPointer>
#include <QElapsedTimer>

#include <memory>
#include <limits>
#include <QObject>
#include <QRunnable>
#include <QMutex>
#include <QReadWriteLock>


namespace Addle {

/**
 * @brief
 * General-purpose exception indicating an interruption of an AsyncTask that is
 * not necessarily a failure or error.
 * 
 * Optionally holds an int code that may be interpreted by task or its owner.
 */
class AsyncInterruption : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(AsyncInterruption);
public:
    AsyncInterruption(int code = -1)
        : 
#ifdef ADDLE_DEBUG
            AddleException(
                //% "An async task was interrupted with code %1"
                qtTrId("debug-messages.async-task.interruption")
                    .arg(code)
            ),
#endif
            _code(code)
    {
    }
    virtual ~AsyncInterruption() = default;
    
    int code() const { return _code; }
private:
    int _code;
};

/**
 * @brief
 * Extensible base class for asynchronous tasks.
 * 
 * Derive from AsyncTask to create a custom asynchronous task *with custom
 * progress reporting, rich error handling, and built-in support for Qt signals
 * and slots.*
 * 
 * Provide the derived class with data members to serve as input and output for
 * the task, and implement `doTask()` to perform the logic. Communicate with
 * AsyncTask using signals and slots.
 * 
 * @todo lock timeouts?
 */
class ADDLE_COMMON_EXPORT AsyncTask : public QObject
{
    Q_OBJECT 

    class Worker : public QRunnable
    {
    public:
        Worker(AsyncTask* owner);
        virtual ~Worker();

        void orphan();
        void run() override;

    private:
        AsyncTask* _owner;
    };

public:
    AsyncTask(QObject* parent = nullptr);
    virtual ~AsyncTask();

    double maxProgress() const { const QReadLocker lock(&_stateLock); return _maxProgress; }
    double minProgress() const { const QReadLocker lock(&_stateLock); return _minProgress; }
    double progress() const { const QReadLocker lock(&_stateLock); return _progress; }

    /**
     * The task has started and has not yet stopped.
     * @warning It is not safe to set some proprerties while the task is running.
     */
    bool isRunning() const { const QReadLocker lock(&_stateLock); return _isRunning; }

    /**
     * The most recent run of the task completed successfully (and stopped). If
     * the task has output data, it should be available now.
     */
    bool hasCompleted() const { const QReadLocker lock(&_stateLock); return _hasCompleted; }

    /**
     * The most recent run of the task stopped because the underlying operation
     * produced an error.
     */
    bool hasFailed() const { const QReadLocker lock(&_stateLock); return _hasFailed; }

    /** 
     * If the most recent run of the task was stopped because of an error, this
     * accesses the error.
     */
    QSharedPointer<AddleException> error() const
    { 
        const QReadLocker lock(&_stateLock); 
        return _hasFailed ? _exception : nullptr; 
    }
    
    QSharedPointer<AsyncInterruption> interruption() const
    { 
        const QReadLocker lock(&_stateLock);
        return _isInterrupted ? _exception.staticCast<AsyncInterruption>() : nullptr; 
    }

    void sync() { const QReadLocker lock(&_stateLock); while (_isRunning); }

    //todo trySync(int timeout = 0);

public slots:
    // Starts the task.
    void start();

signals: 
    // The task started. 
    void started();

    // The task stopped.
    void stopped();

    // The task stopped and was successful.
    void completed();

    // The task stopped because of an error.
    void failed(QSharedPointer<AddleException>);
    
    // The task stopped because it was interrupted.
    void interrupted(int code);

    void maxProgressChanged(double maxProgress);
    void minProgressChanged(double minProgress);
    void progressChanged(double progress);

protected:
    TaskProgressHandle& progressHandle()
    {
        ADDLE_ASSERT(_progressHandle);
        return *_progressHandle;
    }
    
    void setMaxProgress(double maxProgress)
    {
        ADDLE_ASSERT(_progressHandle);
        _progressHandle->setMax(maxProgress);
    }
    
    void setMinProgress(double minProgress)
    {
        ADDLE_ASSERT(_progressHandle);
        _progressHandle->setMin(minProgress);
    }
    
    [[noreturn]] void interrupt(int code = -1) { throw AsyncInterruption(code); }
    
    inline std::unique_ptr<QReadLocker> lockRead() const
    {
        return std::unique_ptr<QReadLocker>(new QReadLocker(&_ioLock));
    }
    
    inline std::unique_ptr<QWriteLocker> lockWrite() const
    {
        {
            const QReadLocker stateLock(&_stateLock);
            ADDLE_ASSERT(!_isRunning);
            // Pre-conditions should not be changed while the task is running.
        }
        return std::unique_ptr<QWriteLocker>(new QWriteLocker(&_ioLock));
    }
    
    /**
     * Implement this function in the derived task and do the logic from there.
     * It is run on the worker thread.
     * 
     * Throwing exceptions from this function is safe. `this` will remain valid
     * and no preconditions guarded by lockWrite() will change.
     */
    virtual void doTask() = 0;
    
    /**
     * Reimplement these functions from the derived task to respond to the
     * respective circumstances. They are run on the worker thread.
     */
    virtual void onSuccess() {}
    virtual void onInterrupted() {}
    virtual void onError() {}

private slots:
    void onProgressHandleUpdated();
    
private:
    bool _isRunning = false;
    bool _hasCompleted = false;
    bool _hasFailed = false;
    bool _isInterrupted = false;

    QSharedPointer<AddleException> _exception;

    std::unique_ptr<TaskProgressHandle> _progressHandle;
    
    double _maxProgress = 1.0;
    double _minProgress = 0.0;
    double _progress = 0.0;
    
    QElapsedTimer _elapsedTimer;
    qint64 _elapsed             = 0;
    qint64 _lastProgressElapsed = 0;
    
    int _maxProgressRefreshRate = 60;
        
    // These manage concurrent access to IO (i.e., the pre- and post-conditions
    // of the task) and state respectively.
    
    // public-facing property getters and setters on derived tasks should
    // use the protected lockRead and lockWrite methods. 
    mutable QReadWriteLock _ioLock;
    mutable QReadWriteLock _stateLock;
    
    // This mutex is locked while the task is running and blocks it or its
    // worker from being deleted or orphaned.
    mutable QMutex _isRunningMutex;

    Worker* _worker = nullptr;
    friend class Worker;
};

} // namespace Addle

#endif // ASYNCTASK_HPP
