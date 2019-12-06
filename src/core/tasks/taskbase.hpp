#ifndef TASKHELPERBASE_HPP
#define TASKHELPERBASE_HPP

#include "interfaces/tasks/itask.hpp"

class TaskBase: public virtual ITask
{
public:
    TaskBase();
    virtual ~TaskBase() = default;

    virtual QSharedPointer<ITaskController> getController() { return _controller; }

protected:
    QSharedPointer<ITaskController> _controller;
};

#endif // TASKHELPERBASE_HPP