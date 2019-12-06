#include "taskservice.hpp"
#include <QtDebug>
#include <cstdlib>

#include "utilities/qt_extensions/qobject.hpp"
#include "utilities/qt_extensions/translation.hpp"

#include "servicelocator.hpp"

#ifdef ADDLE_DEBUG
#include "utilities/debugging.hpp"
#endif

TaskService::TaskService()
    : _threadHelper(this)
{
}

TaskService::~TaskService()
{
    //TODO
}

void TaskService::queueTask(ITask* task)
{
    _taskQueue.enqueue(task);

    if (!_currentTask)
    {
        dequeueTask();
    }
}

void TaskService::dequeueTask()
{
    if (_taskQueue.isEmpty())
        return;

    _currentTask = _taskQueue.dequeue();

    auto controller = _currentTask->getController();
    QObject* q_controller = qobject_interface_cast(controller.data());
    q_controller->moveToThread(_threadHelper.getServiceThread());

    connect_interface(this, SIGNAL(triggerRunTask()), controller.data(), SLOT(run()));
    connect_interface(controller.data(), SIGNAL(done(ITask*)), this, SLOT(onTaskDone(ITask*)));

    emit triggerRunTask();
    //QMetaObject::invokeMethod(controller, &TaskController::run);
}

void TaskService::onTaskDone(ITask* task)
{
    //assert task is current task;

    delete _currentTask;
    _currentTask = nullptr;

    if (!_taskQueue.isEmpty())
    {
        dequeueTask();
    }
}