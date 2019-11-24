#include "taskbase.hpp"
#include "common/servicelocator.hpp"

TaskBase::TaskBase()
{
    _controller = QSharedPointer<ITaskController>(ServiceLocator::make<ITaskController>(this));
}