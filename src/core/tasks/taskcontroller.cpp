#include "taskcontroller.hpp"

void TaskController::initialize(ITask* task)
{
    _initHelper.initializeBegin();

    _task = task;

    _initHelper.initializeEnd();
}

void TaskController::run()
{
    _initHelper.check();

    emit started(_task);
    try
    {
        _task->run();
    }
    catch(IAddleException& ex)
    {
        _error = QSharedPointer<IAddleException>(ex.clone());
    }
    emit done(_task);
}