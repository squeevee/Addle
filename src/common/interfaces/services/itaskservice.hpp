#ifndef ITASKSERVICE_HPP
#define ITASKSERVICE_HPP

#include "iservice.hpp"

#include <QSharedPointer>

#include "interfaces/tasks/itask.hpp"

#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

#include "interfaces/tasks/itaskcontroller.hpp"

class ITaskService : public virtual IService
{
public:
    virtual ~ITaskService() = default;

    virtual void queueTask(ITask* task) = 0;
};

DECL_MAKEABLE(ITaskService)
DECL_IMPLEMENTED_AS_QOBJECT(ITaskService)

#endif // ITASKSERVICE_HPP