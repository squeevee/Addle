#ifndef TASKCONTROLLER_HPP
#define TASKCONTROLLER_HPP

#include <QObject>
#include <QSharedPointer>

#include "utilities/initializehelper.hpp"

#include "exceptions/baseaddleexception.hpp"
#include "interfaces/tasks/itask.hpp"
#include "interfaces/tasks/itaskcontroller.hpp"
#include "interfaces/tasks/itaskstatuscontroller.hpp"

class TaskController : public QObject, public virtual ITaskController, public virtual ITaskStatusController
{
    Q_OBJECT
    Q_INTERFACES(ITaskController)

public:
    TaskController() : _initHelper(this) {}
    virtual ~TaskController() = default;

    void initialize(ITask* task);

    ITask* getTask() { _initHelper.assertInitialized(); return _task; }

    QSharedPointer<IAddleException> getError() { _initHelper.assertInitialized(); return _error; }

    double getProgress() { return _progress; }
    void setProgress(double progress) { _progress = progress; }

    void postMessage(QSharedPointer<ITaskMessage> message) { _messages.append(message); }

signals:
    void started(ITask* task);
    void done(ITask* task);

public slots:
    void run();

private:
    ITask* _task;
    QSharedPointer<IAddleException> _error;
    double _progress = 0;

    QList<QSharedPointer<ITaskMessage>> _messages;

    InitializeHelper<TaskController> _initHelper;
};

#endif // TASKCONTROLLER_HPP