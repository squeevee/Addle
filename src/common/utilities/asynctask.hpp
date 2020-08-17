#ifndef ASYNCTASK_HPP
#define ASYNCTASK_HPP

#include "compat.hpp"
#include "exceptions/addleexception.hpp"

#include <QSharedPointer>

#include <memory>
#include <QObject>
#include <QRunnable>
#include <QMutex>
namespace Addle {

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
 * I need to read up on async programming techniques to tighten this up.
 * 
 * @todo
 * Allow tasks to report non-fatal diagnostic messages.
 * 
 * @todo
 * In the future, we may want some kind of task pool that can aggregate the
 * progress of multiple parallel tasks for the benefit of the UI.
 * 
 * @todo
 * Tools like QFuture and AsyncFuture that may be useful in the future if
 * complex async logic like daisy-chaining or parallel computation are desired. 
 * 
 * @todo
 * The user is likely to want the ability to cancel tasks if they're very long-
 * running, and/or tasks should support a timeout. This will be a challenge to
 * generalize gracefully, but conservative mutex locking and frequent checking
 * against interrupt requests, could be a start. It may be possible to design
 * some low-level operations for safe thread termination, i.e., by leaning
 * heavily on the stack and atomics, or exposing post-termination cleanup
 * functions.
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

    double maxProgress() { const QMutexLocker lock(&_ioMutex); return _maxProgress; }
    double minProgress() { const QMutexLocker lock(&_ioMutex); return _minProgress; }
    double progress() { const QMutexLocker lock(&_ioMutex); return _progress; }

    // The task has started and has not yet stopped.
    bool isRunning() { const QMutexLocker lock(&_stateMutex); return _isRunning; }

    /**
     * The most recent run of the task completed successfully (and stopped). If
     * the task has output data, it should be available now.
     */
    bool isCompleted() { const QMutexLocker lock(&_stateMutex); return _isCompleted; }

    /**
     * The most recent run of the task stopped because the underlying operation
     * produced an error.
     */
    bool isError() { const QMutexLocker lock(&_stateMutex); return _isError; }

    /** 
     * If the most recent run of the task was stopped because of an error, this
     * accesses the error.
     */
    QSharedPointer<AddleException> error() { const QMutexLocker lock(&_stateMutex); return _error; }

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
    void error(QSharedPointer<AddleException>);

    // todo: Rate-limit these signals like QFuture does
    void maxProgressChanged(double maxProgress);
    void minProgressChanged(double minProgress);
    void progressChanged(double progress);

protected:
    // Use these functions from within doTask() to control the output of the
    // task.
    double setMaxProgress(double maxProgress);
    double setMinProgress(double minProgress);
    double setProgress(double progress);

    inline std::unique_ptr<QMutexLocker> lockIO()
    {
        return std::unique_ptr<QMutexLocker>(new QMutexLocker(&_ioMutex));
    }

    // If doTask returns without throwing an exception, it is assumed to have
    // completed successfully.
    virtual void doTask() = 0;

private:
    bool _isRunning = false;
    bool _isCompleted = false;
    bool _isError = false;

    QSharedPointer<AddleException> _error;

    double _maxProgress = 1.0;
    double _minProgress = 0.0;
    double _progress = 0.0;

    QMutex _ioMutex;
    QMutex _stateMutex;

    Worker* _worker = nullptr;
    friend class Worker;
};
} // namespace Addle

#endif // ASYNCTASK_HPP