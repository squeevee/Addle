#include "taskbase.hpp"
#include "servicelocator.hpp"

TaskBase::TaskBase()
{
    _controller = QSharedPointer<ITaskController>(ServiceLocator::make<ITaskController>(this));
}