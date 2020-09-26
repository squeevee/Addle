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

#include <QSharedPointer>

#include <memory>
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
 * @todo
 * Honestly this class is a mess with potential race conditions. It needs some
 * design TLC
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

    double maxProgress() const { const QMutexLocker lock(&_stateMutex); return _maxProgress; }
    double minProgress() const { const QMutexLocker lock(&_stateMutex); return _minProgress; }
    double progress() const { const QMutexLocker lock(&_stateMutex); return _progress; }

    // The task has started and has not yet stopped.
    bool isRunning() const { const QMutexLocker lock(&_stateMutex); return _isRunning; }

    /**
     * The most recent run of the task completed successfully (and stopped). If
     * the task has output data, it should be available now.
     */
    bool hasCompleted() const { const QMutexLocker lock(&_stateMutex); return _hasCompleted; }

    /**
     * The most recent run of the task stopped because the underlying operation
     * produced an error.
     */
    bool hasFailed() const { const QMutexLocker lock(&_stateMutex); return _hasFailed; }

    /** 
     * If the most recent run of the task was stopped because of an error, this
     * accesses the error.
     */
    QSharedPointer<AddleException> error() const { const QMutexLocker lock(&_stateMutex); return _error; }
    
    QSharedPointer<AsyncInterruption> interruption() const { const QMutexLocker lock(&_stateMutex); return _interruption; }

    void sync() { const QMutexLocker lock(&_stateMutex); while (_isRunning); }

    //todo trySync(int timeout = 0);

public slots:
    // Starts the task.
    void start();

signals: 
    // The task has started. 
    void started();

    // The task has stopped.
    void stopped();

    // The task has stopped and has successfully completed.
    void completed();

    // The task has stopped because of an error.
    void failed(QSharedPointer<AddleException>);
    
    // The task has stopped because it was interrupted.
    void interrupted(int code);

    // todo: Rate-limit these signals like QFuture does
    void maxProgressChanged(double maxProgress);
    void minProgressChanged(double minProgress);
    void progressChanged(double progress);

protected:
    // Use these functions from within doTask() to control the output of the
    // task.
    void setMaxProgress(double maxProgress);
    void setMinProgress(double minProgress);
    void setProgress(double progress);
    
    [[noreturn]] void interrupt(int code = -1) { throw AsyncInterruption(code); }
    
    inline std::unique_ptr<QReadLocker> lockRead() const
    {
        return std::unique_ptr<QReadLocker>(new QReadLocker(&_ioLock));
    }
    
    inline std::unique_ptr<QWriteLocker> lockWrite() const
    {
        {
            const QMutexLocker stateLock(&_stateMutex);
            ADDLE_ASSERT(!_isRunning);
        }
        return std::unique_ptr<QWriteLocker>(new QWriteLocker(&_ioLock));
    }
    
    // If doTask returns without throwing an exception, it is assumed to have
    // completed successfully.
    virtual void doTask() = 0;

private:
    bool _isRunning = false;
    bool _hasCompleted = false;
    bool _hasFailed = false;
    bool _isInterrupted = false;

    QSharedPointer<AsyncInterruption> _interruption;
    QSharedPointer<AddleException> _error;

    double _maxProgress = 1.0;
    double _minProgress = 0.0;
    double _progress = 0.0;

    mutable QReadWriteLock _ioLock;
    mutable QMutex _stateMutex;
    mutable QMutex _isRunningMutex;

    Worker* _worker = nullptr;
    friend class Worker;
};

} // namespace Addle

#endif // ASYNCTASK_HPP
