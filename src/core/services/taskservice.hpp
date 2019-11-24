#ifndef TASKSERVICE_HPP
#define TASKSERVICE_HPP

#include <QObject>
#include <QQueue>
#include <QThread>

#include "common/interfaces/services/itaskservice.hpp"
#include "common/interfaces/tasks/itask.hpp"
#include "servicebase.hpp"

#include "helpers/servicethreadhelper.hpp"

class TaskService : public QObject, public virtual ServiceBase, public virtual ITaskService
{
    Q_OBJECT
public:
    TaskService();
    virtual ~TaskService();

    void queueTask(ITask* task);

signals:
    void triggerRunTask();

private:
    void dequeueTask();

    ITask* _currentTask = nullptr;
    QQueue<ITask*> _taskQueue;

    ServiceThreadHelper<TaskService> _threadHelper;

private slots:

    void onTaskDone(ITask*);
};

#endif // TASKSERVICE_HPP